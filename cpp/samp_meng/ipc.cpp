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
// Target: samp_meng (exe) -- Sample matching engine
// Exceptions: yes
// Source: cpp/samp_meng/ipc.cpp
//

#include "include/algo.h"
#include "include/samp_meng.h"
#include "include/lib_netio.h"

// IpcInit -- create Unix domain socket, bind, listen, register with event loop
void samp_meng::IpcInit() {
    algo::Fildes fd = lib_netio::CreateUnixSocket();
    if (!ValidQ(fd)) {
        FatalErrorExit("samp_meng.socket_create_failed");
    }
    tempstr path;
    path << "/tmp/samp_meng." << getpid() << ".sock";
    if (!lib_netio::BindUnix(fd, path)) {
        FatalErrorExit(Zeroterm(tempstr() << "samp_meng.bind_failed  path:" << path));
    }
    if (!lib_netio::Listen(fd, 5)) {
        FatalErrorExit(Zeroterm(tempstr() << "samp_meng.listen_failed  path:" << path));
    }
    algo::SetBlockingMode(fd, false);
    _db.ipc_socket_path = path;
    Zeroterm(_db.ipc_socket_path); // ensure null-terminated for signal handler
    _db.ipc_listen.fildes = fd;
    callback_Set0(_db.ipc_listen, samp_meng::IpcAccept);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(_db.ipc_listen, flags);
    atexit(samp_meng::IpcCleanup);
    struct sigaction sa;
    sa.sa_handler = samp_meng::IpcSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

// Ipc_RequestStateDump -- handle state dump request from IPC client
void samp_meng::Ipc_RequestStateDump(samp_meng::FIpcconn& conn, samp_meng::RequestStateDump& cmd) {
    algo_lib::Regx filter;
    Regx_ReadSql(filter, cmd.filter, true);
    algo::cstring out;
    samp_meng::StateDump(out, filter);
    ssize_t nw = write(conn.outfd.value, out.ch_elems, out.ch_n);
    (void)nw;
}

// IpcAccept -- accept incoming connection, allocate FIpcconn, start reading
void samp_meng::IpcAccept() {
    algo::Fildes client_fd = lib_netio::AcceptUnix(_db.ipc_listen.fildes);
    if (ValidQ(client_fd)) {
        algo::SetBlockingMode(client_fd, false);
        samp_meng::FIpcconn& conn = samp_meng::ipcconn_Alloc();
        conn.outfd = client_fd;
        in_BeginRead(conn, client_fd);
        ipcconn_XrefMaybe(conn);
    }
}
