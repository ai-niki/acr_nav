// Copyright (C) 2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/state_dump.cpp
//

#include "include/amc.h"

// Determine if a field should appear in state dump output.
// Structural check only — HasStringPrintQ is checked at emission time,
// matching GenPrintField's pattern (cpp/amc/print.cpp:136).
static bool StateDumpFieldQ(amc::FField& field) {
    bool good = false;
    good = good || field.reftype == dmmeta_Reftype_reftype_Val;
    good = good || field.reftype == dmmeta_Reftype_reftype_Smallstr;
    good = good || field.reftype == dmmeta_Reftype_reftype_Bitfld;
    good = good || field.reftype == dmmeta_Reftype_reftype_Regx;
    good = good && field.reftype != dmmeta_Reftype_reftype_Base;
    good = good && field.arg != "pad_byte";
    good = good && !field.c_substr;
    good = good && (!field.c_cppfunc || field.c_cppfunc->print);
    good = good && !field.c_pmaskfld;
    good = good && !field.c_typefld;
    good = good && !GetLenfld(field);
    bool source = !bh_bitfld_EmptyQ(field);
    good = good && !source;  // exclude bitfield backing fields in Tuple mode
    return good;
}

// For a Ptr field, resolve the pkey field of the target ctype
// by following the Base chain to find a ctype with c_pkeyfield set.
// Ptr fields use direct member access (ptr->pkeyname) rather than
// FieldvalExpr, since !FldfuncQ guarantees simple member access.
static amc::FField *ResolvePtrPkey(amc::FField& ptr_field) {
    amc::FCtype *ctype = ptr_field.p_arg;
    while (ctype && !ctype->c_pkeyfield) {
        amc::FCtype *base = amc::GetBaseType(*ctype, NULL);
        if (!base) break;
        ctype = base;
    }
    return ctype ? ctype->c_pkeyfield : NULL;
}

// For each namespace with an nsdump record, generate a StateDump function
// that emits a pool census (record count per pool) plus filtered record dump.
// Pools whose arg ctype has a cfmt with print:Y use the cfmt printer.
// Pools without cfmt get field-by-field serialization.
// Lary, Inlary, and Tary pools are covered — Tpool/Lpool/Sbrk have no cursor.
void amc::gen_ns_state_dump() {
    amc::FNs &ns = *amc::_db.c_ns;
    if (ns.c_nsdump && ns.c_globfld) {
        amc::FCtype *fdb = ns.c_globfld->p_arg;
        algo_lib::Replscope R;
        Set(R, "$ns", ns.ns);
        amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...StateDump"));
        func.glob = true;
        func.extrn = false;
        func.ret = "void";
        func.proto = "StateDump(algo::cstring& out, algo_lib::Regx& filter)";
        Ins(&R, func.body, "report::PoolCensus census;");
        Ins(&R, func.body, "(void)filter;");
        // FDb singleton dump -- scalar fields on the global _db
        Set(R, "$Fdbctype", fdb->ctype);
        {
            int n_emittable = 0;
            ind_beg(amc::ctype_c_field_curs, fdb_field, *fdb) {
                if (StateDumpFieldQ(fdb_field)) {
                    amc::FFunc* fdb_print = amc::ind_func_Find(dmmeta::Func_Concat_field_name(fdb_field.field, "Print"));
                    bool has_custom_print = fdb_print && !fdb_print->ismacro;
                    bool has_type_print = amc::HasStringPrintQ(*fdb_field.p_arg);
                    if (has_custom_print || has_type_print) {
                        n_emittable++;
                    }
                }
            }ind_end;
            // Ptr fields — count those with resolvable pkey
            ind_beg(amc::ctype_c_field_curs, fdb_field, *fdb) {
                if (fdb_field.reftype == dmmeta_Reftype_reftype_Ptr
                    && !FldfuncQ(fdb_field)) {
                    amc::FField *pkeyfield = ResolvePtrPkey(fdb_field);
                    if (pkeyfield && amc::HasStringPrintQ(*pkeyfield->p_arg)) {
                        n_emittable++;
                    }
                }
            }ind_end;
            if (n_emittable > 0) {
                Ins(&R, func.body, "if (Regx_Match(filter, strptr(\"$Fdbctype\"))) {");
                Ins(&R, func.body, "    algo::tempstr temp;");
                Ins(&R, func.body, "    out << \"$Fdbctype\";");
                ind_beg(amc::ctype_c_field_curs, fdb_field, *fdb) {
                    if (StateDumpFieldQ(fdb_field)) {
                        Set(R, "$fname", name_Get(fdb_field));
                        amc::FFunc* custom_print = amc::ind_func_Find(dmmeta::Func_Concat_field_name(fdb_field.field, "Print"));
                        if (custom_print && !custom_print->ismacro) {
                            Set(R, "$fns", ns_Get(*fdb));
                            Ins(&R, func.body, "    $fns::$fname_Print($ns::_db, temp);");
                            Ins(&R, func.body, "    PrintAttrSpaceReset(out, \"$fname\", temp);");
                        } else if (amc::HasStringPrintQ(*fdb_field.p_arg)) {
                            Set(R, "$Ftype", fdb_field.p_arg->cpp_type);
                            tempstr access(FieldvalExpr(fdb, fdb_field, Subst(R, "$ns::_db")));
                            Set(R, "$access", access);
                            Ins(&R, func.body, "    $Ftype_Print($access, temp);");
                            Ins(&R, func.body, "    PrintAttrSpaceReset(out, \"$fname\", temp);");
                        }
                    }
                }ind_end;
                // Ptr fields — emit pkey of pointed-to record (empty for NULL)
                ind_beg(amc::ctype_c_field_curs, fdb_field, *fdb) {
                    if (fdb_field.reftype == dmmeta_Reftype_reftype_Ptr && !FldfuncQ(fdb_field)) {
                        amc::FField *pkeyfield = ResolvePtrPkey(fdb_field);
                        if (pkeyfield && amc::HasStringPrintQ(*pkeyfield->p_arg)) {
                            Set(R, "$fname", name_Get(fdb_field));
                            Set(R, "$Pkeytype", pkeyfield->p_arg->cpp_type);
                            Set(R, "$pkeyname", name_Get(*pkeyfield));
                            Ins(&R, func.body, "    if ($ns::_db.$fname) {");
                            Ins(&R, func.body, "        $Pkeytype_Print($ns::_db.$fname->$pkeyname, temp);");
                            Ins(&R, func.body, "    }");
                            Ins(&R, func.body, "    PrintAttrSpaceReset(out, \"$fname\", temp);");
                        }
                    }
                }ind_end;
                Ins(&R, func.body, "    out << '\\n';");
                Ins(&R, func.body, "}");
            }
        }
        ind_beg(amc::ctype_c_field_curs, field, *fdb) {
            // Lary, Inlary, and Tary have _N() and cursors
            if (field.reftype == dmmeta_Reftype_reftype_Lary
                || field.reftype == dmmeta_Reftype_reftype_Inlary
                || field.reftype == dmmeta_Reftype_reftype_Tary) {
                Set(R, "$name", name_Get(field));
                Set(R, "$Cpptype", field.p_arg->cpp_type);
                Set(R, "$ctype", field.p_arg->ctype);
                Ins(&R, func.body, "census.ctype = \"$ctype\";");
                Ins(&R, func.body, "census.n_record = $ns::$name_N();");
                Ins(&R, func.body, "report::PoolCensus_Print(census, out);");
                Ins(&R, func.body, "out << '\\n';");
                // Check if the pool's arg ctype has a cfmt with print:Y
                bool printable = false;
                ind_beg(amc::ctype_zs_cfmt_curs, cfmt, *field.p_arg) {
                    if (cfmt.print) {
                        printable = true;
                        break;
                    }
                }ind_end;
                if (printable) {
                    Ins(&R, func.body, "if (Regx_Match(filter, strptr(\"$ctype\"))) {");
                    Ins(&R, func.body, "    ind_beg($ns::_db_$name_curs, rec, $ns::_db) {");
                    Ins(&R, func.body, "        $Cpptype_Print(rec, out);");
                    Ins(&R, func.body, "        out << '\\n';");
                    Ins(&R, func.body, "    }ind_end;");
                    Ins(&R, func.body, "}");
                } else {
                    // Field-by-field serialization for pools without cfmt
                    // Count emittable fields at generation time
                    int n_emittable = 0;
                    ind_beg(amc::ctype_c_field_curs, inner_field, *field.p_arg) {
                        if (StateDumpFieldQ(inner_field)) {
                            amc::FFunc* count_print = amc::ind_func_Find(dmmeta::Func_Concat_field_name(inner_field.field, "Print"));
                            bool has_custom_print = count_print && !count_print->ismacro;
                            bool has_type_print = amc::HasStringPrintQ(*inner_field.p_arg);
                            if (has_custom_print || has_type_print) {
                                n_emittable++;
                            }
                        }
                    }ind_end;
                    if (n_emittable > 0) {
                        Ins(&R, func.body, "if (Regx_Match(filter, strptr(\"$ctype\"))) {");
                        Ins(&R, func.body, "    ind_beg($ns::_db_$name_curs, rec, $ns::_db) {");
                        Ins(&R, func.body, "        algo::tempstr temp;");
                        Ins(&R, func.body, "        out << \"$ctype\";");
                        ind_beg(amc::ctype_c_field_curs, inner_field, *field.p_arg) {
                            if (StateDumpFieldQ(inner_field)) {
                                Set(R, "$fname", name_Get(inner_field));
                                amc::FFunc* custom_print = amc::ind_func_Find(dmmeta::Func_Concat_field_name(inner_field.field, "Print"));
                                if (custom_print && !custom_print->ismacro) {
                                    Set(R, "$fns", ns_Get(*field.p_arg));
                                    Ins(&R, func.body, "        $fns::$fname_Print(rec, temp);");
                                    Ins(&R, func.body, "        PrintAttrSpaceReset(out, \"$fname\", temp);");
                                } else if (amc::HasStringPrintQ(*inner_field.p_arg)) {
                                    Set(R, "$Ftype", inner_field.p_arg->cpp_type);
                                    tempstr access(FieldvalExpr(field.p_arg, inner_field, "rec"));
                                    Set(R, "$access", access);
                                    Ins(&R, func.body, "        $Ftype_Print($access, temp);");
                                    Ins(&R, func.body, "        PrintAttrSpaceReset(out, \"$fname\", temp);");
                                }
                            }
                        }ind_end;
                        Ins(&R, func.body, "        out << '\\n';");
                        Ins(&R, func.body, "    }ind_end;");
                        Ins(&R, func.body, "}");
                    }
                }
            }
        }ind_end;
    }
}
