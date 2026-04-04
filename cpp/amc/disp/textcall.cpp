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
// Source: cpp/amc/disp/textcall.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Generate text-to-handler dispatch: extrn handler declarations,
// DispatchText function, and optional UnkText handler.
void amc::Disp_TextCall(amc::FDispatch &disp) {
    vrfy(!(disp.call && disp.textcall && disp.haslen)
         , tempstr() << "amc.textcall_haslen"
         <<Keyval("dispatch",disp.dispatch)
         <<Keyval("comment","textcall and call with haslen:Y conflict (signature mismatch)"));

    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$Disp", disp.dispatch);
    Set(R, "$ns", ns_Get(disp));
    amc::FCtype &casetype = *disp.p_casetype;
    amc::FField &typefld = *c_field_Find(casetype,0);
    Set(R, "$Casetype", disp.p_casetype->cpp_type);
    Set(R, "$casetypefld", strptr(name_Get(typefld)));
    Set(R, "$caseenumprefix", (tempstr() << ns_Get(casetype) << "_"<< name_Get(casetype)));

    // (a) Extrn handler declarations for each dispatch_msg
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, disp) {
        Set(R, "$msgname", StripNs("",ctype_Get(dispatch_msg)));
        Set(R, "$msgns", GetNs(ctype_Get(dispatch_msg)));
        amc::FCfmt *cfmt = FindStringRead(*dispatch_msg.p_ctype);
        if (!cfmt) {
            prerr("amc.textcall_no_read"
                  <<Keyval("dispatch_msg",dispatch_msg.dispatch_msg)
                  <<Keyval("comment","No cfmt with read:Y strfmt:String -- cannot generate textcall handler"));
            algo_lib::_db.exit_code=1;
        }
        amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname.$msgns.$msgname"));
        func.acrkey <<"dispatch_msg:"<<dispatch_msg.dispatch_msg;
        func.extrn = true;
        func.ret = "void";
        Ins(&R, func.comment, "User-implemented callback function for dispatch $Disp", false);
        Ins(&R, func.proto, "$Dname_$msgname()", false); {
            amc::AddCtxProtoArg(disp, func);
            amc::AddProtoArg(func, tempstr() << dispatch_msg.p_ctype->cpp_type << " &", "msg");
        }
    }ind_end;

    // (c) When unk:Y, generate extrn UnkText handler
    if (disp.unk) {
        amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..UnkText"));
        func.acrkey <<"dispatch:"<<disp.dispatch;
        func.extrn = true;
        func.ret = "void";
        Ins(&R, func.comment, "User-implemented callback for unrecognized text input to dispatch $Disp", false);
        Ins(&R, func.proto, "$Dname_UnkText()", false); {
            amc::AddCtxProtoArg(disp, func);
            amc::AddProtoArg(func, "algo::strptr", "line");
        }
    }

    // (b) Generate $Dname_DispatchText function
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$Disp..DispatchText"));
    func.ret = "bool";
    func.glob = true;
    Ins(&R, func.comment, "Dispatch text command to the appropriate handler function.", false);
    Ins(&R, func.proto, "$Dname_DispatchText()", false); {
        amc::AddCtxProtoArg(disp, func);
        amc::AddProtoArg(func, "algo::strptr", "line");
    }
    Ins(&R, func.body            , "bool ret = false;");
    Ins(&R, func.body            , "tempstr msgtype_str;");
    Ins(&R, func.body            , "algo::StringIter iter(line);");
    Ins(&R, func.body            , "cstring_ReadCmdarg(msgtype_str, iter, false); // read first word");
    Ins(&R, func.body            , "$Casetype msgtype;");
    Ins(&R, func.body            , "$casetypefld_SetStrptrMaybe(msgtype, msgtype_str); // map string -> enum");
    Ins(&R, func.body            , "switch (value_GetEnum(msgtype)) {");

    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg, disp) {
        Set(R, "$Msgname", name_Get(*msg.p_ctype));
        Set(R, "$msgns", ns_Get(*msg.p_ctype));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        amc::FCfmt *cfmt = FindStringRead(*msg.p_ctype);
        Ins(&R, func.body        , "case $caseenumprefix_$msgns_$Msgname: {");
        if (!cfmt) {
            Ins(&R, func.body    , "    // no cfmt read:Y found -- cannot read");
        } else {
            Ins(&R, func.body    , "    $Ctype msg;");
            Ins(&R, func.body    , "    if ($Msgname_ReadStrptrMaybe(msg, line)) {");
            Ins(&R, func.body    , "        $ns::$Dname_$Msgname();"); {
                amc::AddArg(func.body, "ctx", disp.c_dispctx);
                amc::AddArg(func.body, "msg");
            }
            Ins(&R, func.body    , "        ret = true;");
            Ins(&R, func.body    , "    }");
        }
        Ins(&R, func.body        , "} break;");
        Ins(&R, func.body        , "");
    }ind_end;

    Ins(&R, func.body            , "default: break;");
    Ins(&R, func.body            , "}");

    if (disp.unk) {
        Ins(&R, func.body        , "if (!ret) {");
        Ins(&R, func.body        , "    $ns::$Dname_UnkText();"); {
            amc::AddArg(func.body, "ctx", disp.c_dispctx);
            amc::AddArg(func.body, "line");
        }
        Ins(&R, func.body        , "}");
    }
    Ins(&R, func.body            , "return ret;");
}
