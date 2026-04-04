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
// Target: acr_nav (exe) -- TUI schema explorer for browsing ctypes, fields, and cross-references
// Exceptions: yes
// Source: cpp/acr_nav/ipc.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"
#include "include/lib_netio.h"

// IpcInit -- create Unix domain socket, bind, listen, register with event loop
void acr_nav::IpcInit() {
    algo::Fildes fd = lib_netio::CreateUnixSocket();
    if (!ValidQ(fd)) {
        FatalErrorExit("acr_nav.socket_create_failed");
    }
    tempstr path;
    path << "/tmp/acr_nav." << getpid() << ".sock";
    if (!lib_netio::BindUnix(fd, path)) {
        FatalErrorExit(Zeroterm(tempstr() << "acr_nav.bind_failed  path:" << path));
    }
    if (!lib_netio::Listen(fd, 5)) {
        FatalErrorExit(Zeroterm(tempstr() << "acr_nav.listen_failed  path:" << path));
    }
    algo::SetBlockingMode(fd, false);
    _db.ipc_socket_path = path;
    Zeroterm(_db.ipc_socket_path); // ensure null-terminated for signal handler
    _db.ipc_listen.fildes = fd;
    callback_Set0(_db.ipc_listen, acr_nav::IpcAccept);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(_db.ipc_listen, flags);
    atexit(acr_nav::IpcCleanup);
    struct sigaction sa;
    sa.sa_handler = acr_nav::IpcSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
}

// Ipc_RequestStateDump -- dispatch handler for state dump request
void acr_nav::Ipc_RequestStateDump(acr_nav::FIpcconn& conn, acr_nav::RequestStateDump& cmd) {
    algo_lib::Regx filter;
    Regx_ReadSql(filter, cmd.filter, true);
    algo::cstring out;
    acr_nav::StateDump(out, filter);
    out << "\n";  // empty line = end-of-response sentinel
    // Non-blocking fd requires write loop for large responses
    const char *p = out.ch_elems;
    int remaining = out.ch_n;
    while (remaining > 0) {
        ssize_t nw = write(conn.outfd.value, p, remaining);
        if (nw > 0) {
            p += nw;
            remaining -= nw;
        } else if (nw < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue; // local socket drains fast
        } else {
            break; // connection error
        }
    }
}

// IpcAccept -- accept incoming connection, allocate FIpcconn, start reading
void acr_nav::IpcAccept() {
    algo::Fildes client_fd = lib_netio::AcceptUnix(_db.ipc_listen.fildes);
    if (ValidQ(client_fd)) {
        algo::SetBlockingMode(client_fd, false);
        acr_nav::FIpcconn& conn = acr_nav::ipcconn_Alloc();
        conn.outfd = client_fd;
        in_BeginRead(conn, client_fd);
        ipcconn_XrefMaybe(conn);
    }
}
