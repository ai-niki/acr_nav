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

// For each namespace with an nsdump record, generate a StateDump function
// that emits a pool census (record count per pool) plus filtered record dump
// for pools whose arg ctype has a cfmt with print:Y.
// Only Lary and Inlary pools are covered — Tpool/Lpool/Sbrk have no cursor.
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
        ind_beg(amc::ctype_c_field_curs, field, *fdb) {
            // Only Lary and Inlary pools have _N() and cursors
            if (field.reftype == dmmeta_Reftype_reftype_Lary
                || field.reftype == dmmeta_Reftype_reftype_Inlary) {
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
                }
            }
        }ind_end;
    }
}
