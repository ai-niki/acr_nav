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
// Source: cpp/amc/ipc.cpp
//

#include "include/amc.h"

// For each namespace with both nsipc and nsdump records, generate IPC
// socket listener functions that expose the StateDump interface via
// a Unix domain socket.  The schema already declares FIpcconn, FDb
// fields (ipc_listen, ipc_socket_path, ipcconn pool/list), fbuf, Tpool,
// and Llist -- this generator only fills in the function bodies.
void amc::gen_ns_ipc() {
    amc::FNs &ns = *amc::_db.c_ns;
    if (ns.c_nsipc && ns.c_nsdump && ns.c_globfld) {
        algo_lib::Replscope R;
        Set(R, "$ns", ns.ns);

        // IpcInit -- extern: user provides body (touches lib_netio)
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...IpcInit"));
            func.glob = true;
            func.extrn = true;
            func.ret = "void";
            func.proto = "IpcInit()";
        }

        // IpcAccept -- extern: user provides body (touches lib_netio)
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...IpcAccept"));
            func.glob = true;
            func.extrn = true;
            func.ret = "void";
            func.proto = "IpcAccept()";
        }

        // cd_ipcconn_read_Step -- process one line from a ready connection
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...cd_ipcconn_read_Step"));
            func.glob = true;
            func.extrn = false;
            func.ret = "void";
            func.proto = "cd_ipcconn_read_Step()";
            Ins(&R, func.body, "$ns::FIpcconn& conn = *$ns::cd_ipcconn_read_RotateFirst();");
            Ins(&R, func.body, "algo::strptr line = in_GetMsg(conn);");
            Ins(&R, func.body, "if (line.elems) {");
            Ins(&R, func.body, "    $ns::IpcProcessLine(conn, line);");
            Ins(&R, func.body, "    in_SkipMsg(conn);");
            Ins(&R, func.body, "}");
        }

        // IpcProcessLine -- dispatch a single IPC command
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...IpcProcessLine"));
            func.glob = true;
            func.extrn = false;
            func.ret = "void";
            func.proto = Subst(R, "IpcProcessLine($ns::FIpcconn& conn, algo::strptr line)");
            Ins(&R, func.body, "$ns::Ipc_DispatchText(conn, line);");
        }

        // cd_ipcconn_eof_Step -- clean up a disconnected connection
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...cd_ipcconn_eof_Step"));
            func.glob = true;
            func.extrn = false;
            func.ret = "void";
            func.proto = "cd_ipcconn_eof_Step()";
            Ins(&R, func.body, "$ns::FIpcconn& conn = *$ns::cd_ipcconn_eof_First();");
            Ins(&R, func.body, "close(conn.outfd.value);");
            Ins(&R, func.body, "$ns::ipcconn_Delete(conn);");
        }

        // IpcCleanup -- atexit handler, remove socket file
        {
            amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns...IpcCleanup"));
            func.glob = true;
            func.extrn = false;
            func.ret = "void";
            func.proto = "IpcCleanup()";
            Ins(&R, func.body, "unlink($ns::_db.ipc_socket_path.ch_elems);");
        }
    }
}
