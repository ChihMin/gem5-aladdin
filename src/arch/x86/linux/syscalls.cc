/*
 * Copyright (c) 2007 The Hewlett-Packard Development Company
 * All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * The software must be used only for Non-Commercial Use which means any
 * use which is NOT directed to receiving any direct monetary
 * compensation for, or commercial advantage from such use.  Illustrative
 * examples of non-commercial use are academic research, personal study,
 * teaching, education and corporate research & development.
 * Illustrative examples of commercial use are distributing products for
 * commercial advantage and providing services using the software for
 * commercial advantage.
 *
 * If you wish to use this software or functionality therein that may be
 * covered by patents for commercial use, please contact:
 *     Director of Intellectual Property Licensing
 *     Office of Strategy and Technology
 *     Hewlett-Packard Company
 *     1501 Page Mill Road
 *     Palo Alto, California  94304
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.  Redistributions
 * in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.  Neither the name of
 * the COPYRIGHT HOLDER(s), HEWLETT-PACKARD COMPANY, nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.  No right of
 * sublicense is granted herewith.  Derivatives of the software and
 * output created using the software may be prepared, but only for
 * Non-Commercial Uses.  Derivatives of the software may be shared with
 * others provided: (i) the others agree to abide by the list of
 * conditions herein which includes the Non-Commercial Use restrictions;
 * and (ii) such Derivatives of the software include the above copyright
 * notice to acknowledge the contribution from this software where
 * applicable, this list of conditions and the disclaimer below.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gabe Black
 */

#include "arch/x86/linux/process.hh"
#include "arch/x86/linux/linux.hh"
#include "arch/x86/miscregs.hh"
#include "kern/linux/linux.hh"
#include "sim/syscall_emul.hh"

using namespace X86ISA;

/// Target uname() handler.
static SyscallReturn
unameFunc(SyscallDesc *desc, int callnum, LiveProcess *process,
          ThreadContext *tc)
{
    TypedBufferArg<Linux::utsname> name(process->getSyscallArg(tc, 0));

    strcpy(name->sysname, "Linux");
    strcpy(name->nodename, "m5.eecs.umich.edu");
    strcpy(name->release, "2.6.16.19");
    strcpy(name->version, "#1 Mon Aug 18 11:32:15 EDT 2003");
    strcpy(name->machine, "x86_64");

    name.copyOut(tc->getMemPort());

    return 0;
}

static SyscallReturn
archPrctlFunc(SyscallDesc *desc, int callnum, LiveProcess *process,
              ThreadContext *tc)
{
    enum ArchPrctlCodes
    {
        SetFS = 0x1002,
        GetFS = 0x1003,
        SetGS = 0x1001,
        GetGS = 0x1004
    };

    //First argument is the code, second is the address
    int code = process->getSyscallArg(tc, 0);
    uint64_t addr = process->getSyscallArg(tc, 1);
    uint64_t fsBase, gsBase;
    TranslatingPort *p = tc->getMemPort();
    switch(code)
    {
      //Each of these valid options should actually check addr.
      case SetFS:
        tc->setMiscRegNoEffect(MISCREG_FS_BASE, addr);
        tc->setMiscRegNoEffect(MISCREG_FS_EFF_BASE, addr);
        return 0;
      case GetFS:
        fsBase = tc->readMiscRegNoEffect(MISCREG_FS_BASE);
        p->write(addr, fsBase);
        return 0;
      case SetGS:
        tc->setMiscRegNoEffect(MISCREG_GS_BASE, addr);
        tc->setMiscRegNoEffect(MISCREG_GS_EFF_BASE, addr);
        return 0;
      case GetGS:
        gsBase = tc->readMiscRegNoEffect(MISCREG_GS_BASE);
        p->write(addr, gsBase);
        return 0;
      default:
        return -EINVAL;
    }
}

SyscallDesc X86_64LinuxProcess::syscallDescs[] = {
    /*   0 */ SyscallDesc("read", readFunc),
    /*   1 */ SyscallDesc("write", writeFunc),
    /*   2 */ SyscallDesc("open", openFunc<X86Linux64>),
    /*   3 */ SyscallDesc("close", closeFunc),
    /*   4 */ SyscallDesc("stat", unimplementedFunc),
    /*   5 */ SyscallDesc("fstat", fstat64Func<X86Linux64>),
    /*   6 */ SyscallDesc("lstat", unimplementedFunc),
    /*   7 */ SyscallDesc("poll", unimplementedFunc),
    /*   8 */ SyscallDesc("lseek", lseekFunc),
    /*   9 */ SyscallDesc("mmap", mmapFunc<X86Linux64>),
    /*  10 */ SyscallDesc("mprotect", unimplementedFunc),
    /*  11 */ SyscallDesc("munmap", munmapFunc),
    /*  12 */ SyscallDesc("brk", brkFunc),
    /*  13 */ SyscallDesc("rt_sigaction", unimplementedFunc),
    /*  14 */ SyscallDesc("rt_sigprocmask", unimplementedFunc),
    /*  15 */ SyscallDesc("rt_sigreturn", unimplementedFunc),
    /*  16 */ SyscallDesc("ioctl", unimplementedFunc),
    /*  17 */ SyscallDesc("pread64", unimplementedFunc),
    /*  18 */ SyscallDesc("pwrite64", unimplementedFunc),
    /*  19 */ SyscallDesc("readv", unimplementedFunc),
    /*  20 */ SyscallDesc("writev", writevFunc<X86Linux64>),
    /*  21 */ SyscallDesc("access", unimplementedFunc),
    /*  22 */ SyscallDesc("pipe", unimplementedFunc),
    /*  23 */ SyscallDesc("select", unimplementedFunc),
    /*  24 */ SyscallDesc("sched_yield", unimplementedFunc),
    /*  25 */ SyscallDesc("mremap", mremapFunc<X86Linux64>),
    /*  26 */ SyscallDesc("msync", unimplementedFunc),
    /*  27 */ SyscallDesc("mincore", unimplementedFunc),
    /*  28 */ SyscallDesc("madvise", unimplementedFunc),
    /*  29 */ SyscallDesc("shmget", unimplementedFunc),
    /*  30 */ SyscallDesc("shmat", unimplementedFunc),
    /*  31 */ SyscallDesc("shmctl", unimplementedFunc),
    /*  32 */ SyscallDesc("dup", unimplementedFunc),
    /*  33 */ SyscallDesc("dup2", unimplementedFunc),
    /*  34 */ SyscallDesc("pause", unimplementedFunc),
    /*  35 */ SyscallDesc("nanosleep", unimplementedFunc),
    /*  36 */ SyscallDesc("getitimer", unimplementedFunc),
    /*  37 */ SyscallDesc("alarm", unimplementedFunc),
    /*  38 */ SyscallDesc("setitimer", unimplementedFunc),
    /*  39 */ SyscallDesc("getpid", unimplementedFunc),
    /*  40 */ SyscallDesc("sendfile", unimplementedFunc),
    /*  41 */ SyscallDesc("socket", unimplementedFunc),
    /*  42 */ SyscallDesc("connect", unimplementedFunc),
    /*  43 */ SyscallDesc("accept", unimplementedFunc),
    /*  44 */ SyscallDesc("sendto", unimplementedFunc),
    /*  45 */ SyscallDesc("recvfrom", unimplementedFunc),
    /*  46 */ SyscallDesc("sendmsg", unimplementedFunc),
    /*  47 */ SyscallDesc("recvmsg", unimplementedFunc),
    /*  48 */ SyscallDesc("shutdown", unimplementedFunc),
    /*  49 */ SyscallDesc("bind", unimplementedFunc),
    /*  50 */ SyscallDesc("listen", unimplementedFunc),
    /*  51 */ SyscallDesc("getsockname", unimplementedFunc),
    /*  52 */ SyscallDesc("getpeername", unimplementedFunc),
    /*  53 */ SyscallDesc("socketpair", unimplementedFunc),
    /*  54 */ SyscallDesc("setsockopt", unimplementedFunc),
    /*  55 */ SyscallDesc("getsockopt", unimplementedFunc),
    /*  56 */ SyscallDesc("clone", unimplementedFunc),
    /*  57 */ SyscallDesc("fork", unimplementedFunc),
    /*  58 */ SyscallDesc("vfork", unimplementedFunc),
    /*  59 */ SyscallDesc("execve", unimplementedFunc),
    /*  60 */ SyscallDesc("exit", exitFunc),
    /*  61 */ SyscallDesc("wait4", unimplementedFunc),
    /*  62 */ SyscallDesc("kill", unimplementedFunc),
    /*  63 */ SyscallDesc("uname", unameFunc),
    /*  64 */ SyscallDesc("semget", unimplementedFunc),
    /*  65 */ SyscallDesc("semop", unimplementedFunc),
    /*  66 */ SyscallDesc("semctl", unimplementedFunc),
    /*  67 */ SyscallDesc("shmdt", unimplementedFunc),
    /*  68 */ SyscallDesc("msgget", unimplementedFunc),
    /*  69 */ SyscallDesc("msgsnd", unimplementedFunc),
    /*  70 */ SyscallDesc("msgrcv", unimplementedFunc),
    /*  71 */ SyscallDesc("msgctl", unimplementedFunc),
    /*  72 */ SyscallDesc("fcntl", unimplementedFunc),
    /*  73 */ SyscallDesc("flock", unimplementedFunc),
    /*  74 */ SyscallDesc("fsync", unimplementedFunc),
    /*  75 */ SyscallDesc("fdatasync", unimplementedFunc),
    /*  76 */ SyscallDesc("truncate", unimplementedFunc),
    /*  77 */ SyscallDesc("ftruncate", unimplementedFunc),
    /*  78 */ SyscallDesc("getdents", unimplementedFunc),
    /*  79 */ SyscallDesc("getcwd", unimplementedFunc),
    /*  80 */ SyscallDesc("chdir", unimplementedFunc),
    /*  81 */ SyscallDesc("fchdir", unimplementedFunc),
    /*  82 */ SyscallDesc("rename", renameFunc),
    /*  83 */ SyscallDesc("mkdir", unimplementedFunc),
    /*  84 */ SyscallDesc("rmdir", unimplementedFunc),
    /*  85 */ SyscallDesc("creat", unimplementedFunc),
    /*  86 */ SyscallDesc("link", unimplementedFunc),
    /*  87 */ SyscallDesc("unlink", unlinkFunc),
    /*  88 */ SyscallDesc("symlink", unimplementedFunc),
    /*  89 */ SyscallDesc("readlink", unimplementedFunc),
    /*  90 */ SyscallDesc("chmod", unimplementedFunc),
    /*  91 */ SyscallDesc("fchmod", unimplementedFunc),
    /*  92 */ SyscallDesc("chown", unimplementedFunc),
    /*  93 */ SyscallDesc("fchown", unimplementedFunc),
    /*  94 */ SyscallDesc("lchown", unimplementedFunc),
    /*  95 */ SyscallDesc("umask", unimplementedFunc),
    /*  96 */ SyscallDesc("gettimeofday", unimplementedFunc),
    /*  97 */ SyscallDesc("getrlimit", unimplementedFunc),
    /*  98 */ SyscallDesc("getrusage", unimplementedFunc),
    /*  99 */ SyscallDesc("sysinfo", unimplementedFunc),
    /* 100 */ SyscallDesc("times", unimplementedFunc),
    /* 101 */ SyscallDesc("ptrace", unimplementedFunc),
    /* 102 */ SyscallDesc("getuid", getuidFunc),
    /* 103 */ SyscallDesc("syslog", unimplementedFunc),
    /* 104 */ SyscallDesc("getgid", getgidFunc),
    /* 105 */ SyscallDesc("setuid", unimplementedFunc),
    /* 106 */ SyscallDesc("setgid", unimplementedFunc),
    /* 107 */ SyscallDesc("geteuid", geteuidFunc),
    /* 108 */ SyscallDesc("getegid", getegidFunc),
    /* 109 */ SyscallDesc("setpgid", unimplementedFunc),
    /* 110 */ SyscallDesc("getppid", unimplementedFunc),
    /* 111 */ SyscallDesc("getpgrp", unimplementedFunc),
    /* 112 */ SyscallDesc("setsid", unimplementedFunc),
    /* 113 */ SyscallDesc("setreuid", unimplementedFunc),
    /* 114 */ SyscallDesc("setregid", unimplementedFunc),
    /* 115 */ SyscallDesc("getgroups", unimplementedFunc),
    /* 116 */ SyscallDesc("setgroups", unimplementedFunc),
    /* 117 */ SyscallDesc("setresuid", unimplementedFunc),
    /* 118 */ SyscallDesc("getresuid", unimplementedFunc),
    /* 119 */ SyscallDesc("setresgid", unimplementedFunc),
    /* 120 */ SyscallDesc("getresgid", unimplementedFunc),
    /* 121 */ SyscallDesc("getpgid", unimplementedFunc),
    /* 122 */ SyscallDesc("setfsuid", unimplementedFunc),
    /* 123 */ SyscallDesc("setfsgid", unimplementedFunc),
    /* 124 */ SyscallDesc("getsid", unimplementedFunc),
    /* 125 */ SyscallDesc("capget", unimplementedFunc),
    /* 126 */ SyscallDesc("capset", unimplementedFunc),
    /* 127 */ SyscallDesc("rt_sigpending", unimplementedFunc),
    /* 128 */ SyscallDesc("rt_sigtimedwait", unimplementedFunc),
    /* 129 */ SyscallDesc("rt_sigqueueinfo", unimplementedFunc),
    /* 130 */ SyscallDesc("rt_sigsuspend", unimplementedFunc),
    /* 131 */ SyscallDesc("sigaltstack", unimplementedFunc),
    /* 132 */ SyscallDesc("utime", unimplementedFunc),
    /* 133 */ SyscallDesc("mknod", unimplementedFunc),
    /* 134 */ SyscallDesc("uselib", unimplementedFunc),
    /* 135 */ SyscallDesc("personality", unimplementedFunc),
    /* 136 */ SyscallDesc("ustat", unimplementedFunc),
    /* 137 */ SyscallDesc("statfs", unimplementedFunc),
    /* 138 */ SyscallDesc("fstatfs", unimplementedFunc),
    /* 139 */ SyscallDesc("sysfs", unimplementedFunc),
    /* 140 */ SyscallDesc("getpriority", unimplementedFunc),
    /* 141 */ SyscallDesc("setpriority", unimplementedFunc),
    /* 142 */ SyscallDesc("sched_setparam", unimplementedFunc),
    /* 143 */ SyscallDesc("sched_getparam", unimplementedFunc),
    /* 144 */ SyscallDesc("sched_setscheduler", unimplementedFunc),
    /* 145 */ SyscallDesc("sched_getscheduler", unimplementedFunc),
    /* 146 */ SyscallDesc("sched_get_priority_max", unimplementedFunc),
    /* 147 */ SyscallDesc("sched_get_priority_min", unimplementedFunc),
    /* 148 */ SyscallDesc("sched_rr_get_interval", unimplementedFunc),
    /* 149 */ SyscallDesc("mlock", unimplementedFunc),
    /* 150 */ SyscallDesc("munlock", unimplementedFunc),
    /* 151 */ SyscallDesc("mlockall", unimplementedFunc),
    /* 152 */ SyscallDesc("munlockall", unimplementedFunc),
    /* 153 */ SyscallDesc("vhangup", unimplementedFunc),
    /* 154 */ SyscallDesc("modify_ldt", unimplementedFunc),
    /* 155 */ SyscallDesc("pivot_root", unimplementedFunc),
    /* 156 */ SyscallDesc("_sysctl", unimplementedFunc),
    /* 157 */ SyscallDesc("prctl", unimplementedFunc),
    /* 158 */ SyscallDesc("arch_prctl", archPrctlFunc),
    /* 159 */ SyscallDesc("adjtimex", unimplementedFunc),
    /* 160 */ SyscallDesc("setrlimit", unimplementedFunc),
    /* 161 */ SyscallDesc("chroot", unimplementedFunc),
    /* 162 */ SyscallDesc("sync", unimplementedFunc),
    /* 163 */ SyscallDesc("acct", unimplementedFunc),
    /* 164 */ SyscallDesc("settimeofday", unimplementedFunc),
    /* 165 */ SyscallDesc("mount", unimplementedFunc),
    /* 166 */ SyscallDesc("umount2", unimplementedFunc),
    /* 167 */ SyscallDesc("swapon", unimplementedFunc),
    /* 168 */ SyscallDesc("swapoff", unimplementedFunc),
    /* 169 */ SyscallDesc("reboot", unimplementedFunc),
    /* 170 */ SyscallDesc("sethostname", unimplementedFunc),
    /* 171 */ SyscallDesc("setdomainname", unimplementedFunc),
    /* 172 */ SyscallDesc("iopl", unimplementedFunc),
    /* 173 */ SyscallDesc("ioperm", unimplementedFunc),
    /* 174 */ SyscallDesc("create_module", unimplementedFunc),
    /* 175 */ SyscallDesc("init_module", unimplementedFunc),
    /* 176 */ SyscallDesc("delete_module", unimplementedFunc),
    /* 177 */ SyscallDesc("get_kernel_syms", unimplementedFunc),
    /* 178 */ SyscallDesc("query_module", unimplementedFunc),
    /* 179 */ SyscallDesc("quotactl", unimplementedFunc),
    /* 180 */ SyscallDesc("nfsservctl", unimplementedFunc),
    /* 181 */ SyscallDesc("getpmsg", unimplementedFunc),
    /* 182 */ SyscallDesc("putpmsg", unimplementedFunc),
    /* 183 */ SyscallDesc("afs_syscall", unimplementedFunc),
    /* 184 */ SyscallDesc("tuxcall", unimplementedFunc),
    /* 185 */ SyscallDesc("security", unimplementedFunc),
    /* 186 */ SyscallDesc("gettid", unimplementedFunc),
    /* 187 */ SyscallDesc("readahead", unimplementedFunc),
    /* 188 */ SyscallDesc("setxattr", unimplementedFunc),
    /* 189 */ SyscallDesc("lsetxattr", unimplementedFunc),
    /* 190 */ SyscallDesc("fsetxattr", unimplementedFunc),
    /* 191 */ SyscallDesc("getxattr", unimplementedFunc),
    /* 192 */ SyscallDesc("lgetxattr", unimplementedFunc),
    /* 193 */ SyscallDesc("fgetxattr", unimplementedFunc),
    /* 194 */ SyscallDesc("listxattr", unimplementedFunc),
    /* 195 */ SyscallDesc("llistxattr", unimplementedFunc),
    /* 196 */ SyscallDesc("flistxattr", unimplementedFunc),
    /* 197 */ SyscallDesc("removexattr", unimplementedFunc),
    /* 198 */ SyscallDesc("lremovexattr", unimplementedFunc),
    /* 199 */ SyscallDesc("fremovexattr", unimplementedFunc),
    /* 200 */ SyscallDesc("tkill", unimplementedFunc),
    /* 201 */ SyscallDesc("time", unimplementedFunc),
    /* 202 */ SyscallDesc("futex", unimplementedFunc),
    /* 203 */ SyscallDesc("sched_setaffinity", unimplementedFunc),
    /* 204 */ SyscallDesc("sched_getaffinity", unimplementedFunc),
    /* 205 */ SyscallDesc("set_thread_area", unimplementedFunc),
    /* 206 */ SyscallDesc("io_setup", unimplementedFunc),
    /* 207 */ SyscallDesc("io_destroy", unimplementedFunc),
    /* 208 */ SyscallDesc("io_getevents", unimplementedFunc),
    /* 209 */ SyscallDesc("io_submit", unimplementedFunc),
    /* 210 */ SyscallDesc("io_cancel", unimplementedFunc),
    /* 211 */ SyscallDesc("get_thread_area", unimplementedFunc),
    /* 212 */ SyscallDesc("lookup_dcookie", unimplementedFunc),
    /* 213 */ SyscallDesc("epoll_create", unimplementedFunc),
    /* 214 */ SyscallDesc("epoll_ctl_old", unimplementedFunc),
    /* 215 */ SyscallDesc("epoll_wait_old", unimplementedFunc),
    /* 216 */ SyscallDesc("remap_file_pages", unimplementedFunc),
    /* 217 */ SyscallDesc("getdents64", unimplementedFunc),
    /* 218 */ SyscallDesc("set_tid_address", unimplementedFunc),
    /* 219 */ SyscallDesc("restart_syscall", unimplementedFunc),
    /* 220 */ SyscallDesc("semtimedop", unimplementedFunc),
    /* 221 */ SyscallDesc("fadvise64", unimplementedFunc),
    /* 222 */ SyscallDesc("timer_create", unimplementedFunc),
    /* 223 */ SyscallDesc("timer_settime", unimplementedFunc),
    /* 224 */ SyscallDesc("timer_gettime", unimplementedFunc),
    /* 225 */ SyscallDesc("timer_getoverrun", unimplementedFunc),
    /* 226 */ SyscallDesc("timer_delete", unimplementedFunc),
    /* 227 */ SyscallDesc("clock_settime", unimplementedFunc),
    /* 228 */ SyscallDesc("clock_gettime", unimplementedFunc),
    /* 229 */ SyscallDesc("clock_getres", unimplementedFunc),
    /* 230 */ SyscallDesc("clock_nanosleep", unimplementedFunc),
    /* 231 */ SyscallDesc("exit_group", exitFunc),
    /* 232 */ SyscallDesc("epoll_wait", unimplementedFunc),
    /* 233 */ SyscallDesc("epoll_ctl", unimplementedFunc),
    /* 234 */ SyscallDesc("tgkill", unimplementedFunc),
    /* 235 */ SyscallDesc("utimes", unimplementedFunc),
    /* 236 */ SyscallDesc("vserver", unimplementedFunc),
    /* 237 */ SyscallDesc("mbind", unimplementedFunc),
    /* 238 */ SyscallDesc("set_mempolicy", unimplementedFunc),
    /* 239 */ SyscallDesc("get_mempolicy", unimplementedFunc),
    /* 240 */ SyscallDesc("mq_open", unimplementedFunc),
    /* 241 */ SyscallDesc("mq_unlink", unimplementedFunc),
    /* 242 */ SyscallDesc("mq_timedsend", unimplementedFunc),
    /* 243 */ SyscallDesc("mq_timedreceive", unimplementedFunc),
    /* 244 */ SyscallDesc("mq_notify", unimplementedFunc),
    /* 245 */ SyscallDesc("mq_getsetattr", unimplementedFunc),
    /* 246 */ SyscallDesc("kexec_load", unimplementedFunc),
    /* 247 */ SyscallDesc("waitid", unimplementedFunc),
    /* 248 */ SyscallDesc("add_key", unimplementedFunc),
    /* 249 */ SyscallDesc("request_key", unimplementedFunc),
    /* 250 */ SyscallDesc("keyctl", unimplementedFunc),
    /* 251 */ SyscallDesc("ioprio_set", unimplementedFunc),
    /* 252 */ SyscallDesc("ioprio_get", unimplementedFunc),
    /* 253 */ SyscallDesc("inotify_init", unimplementedFunc),
    /* 254 */ SyscallDesc("inotify_add_watch", unimplementedFunc),
    /* 255 */ SyscallDesc("inotify_rm_watch", unimplementedFunc),
    /* 256 */ SyscallDesc("migrate_pages", unimplementedFunc),
    /* 257 */ SyscallDesc("openat", unimplementedFunc),
    /* 258 */ SyscallDesc("mkdirat", unimplementedFunc),
    /* 259 */ SyscallDesc("mknodat", unimplementedFunc),
    /* 260 */ SyscallDesc("fchownat", unimplementedFunc),
    /* 261 */ SyscallDesc("futimesat", unimplementedFunc),
    /* 262 */ SyscallDesc("newfstatat", unimplementedFunc),
    /* 263 */ SyscallDesc("unlinkat", unimplementedFunc),
    /* 264 */ SyscallDesc("renameat", unimplementedFunc),
    /* 265 */ SyscallDesc("linkat", unimplementedFunc),
    /* 266 */ SyscallDesc("symlinkat", unimplementedFunc),
    /* 267 */ SyscallDesc("readlinkat", unimplementedFunc),
    /* 268 */ SyscallDesc("fchmodat", unimplementedFunc),
    /* 269 */ SyscallDesc("faccessat", unimplementedFunc),
    /* 270 */ SyscallDesc("pselect6", unimplementedFunc),
    /* 271 */ SyscallDesc("ppoll", unimplementedFunc),
    /* 272 */ SyscallDesc("unshare", unimplementedFunc)
};

SyscallDesc I386LinuxProcess::syscallDescs[] = {
    /*   0 */ SyscallDesc("restart_syscall", unimplementedFunc),
    /*   1 */ SyscallDesc("exit", unimplementedFunc),
    /*   2 */ SyscallDesc("fork", unimplementedFunc),
    /*   3 */ SyscallDesc("read", unimplementedFunc),
    /*   4 */ SyscallDesc("write", unimplementedFunc),
    /*   5 */ SyscallDesc("open", openFunc<X86Linux64>),
    /*   6 */ SyscallDesc("close", unimplementedFunc),
    /*   7 */ SyscallDesc("waitpid", unimplementedFunc),
    /*   8 */ SyscallDesc("creat", unimplementedFunc),
    /*   9 */ SyscallDesc("link", unimplementedFunc),
    /*  10 */ SyscallDesc("unlink", unimplementedFunc),
    /*  11 */ SyscallDesc("execve", unimplementedFunc),
    /*  12 */ SyscallDesc("chdir", unimplementedFunc),
    /*  13 */ SyscallDesc("time", unimplementedFunc),
    /*  14 */ SyscallDesc("mknod", unimplementedFunc),
    /*  15 */ SyscallDesc("chmod", unimplementedFunc),
    /*  16 */ SyscallDesc("lchown", unimplementedFunc),
    /*  17 */ SyscallDesc("break", unimplementedFunc),
    /*  18 */ SyscallDesc("oldstat", unimplementedFunc),
    /*  19 */ SyscallDesc("lseek", unimplementedFunc),
    /*  20 */ SyscallDesc("getpid", unimplementedFunc),
    /*  21 */ SyscallDesc("mount", unimplementedFunc),
    /*  22 */ SyscallDesc("umount", unimplementedFunc),
    /*  23 */ SyscallDesc("setuid", unimplementedFunc),
    /*  24 */ SyscallDesc("getuid", unimplementedFunc),
    /*  25 */ SyscallDesc("stime", unimplementedFunc),
    /*  26 */ SyscallDesc("ptrace", unimplementedFunc),
    /*  27 */ SyscallDesc("alarm", unimplementedFunc),
    /*  28 */ SyscallDesc("oldfstat", unimplementedFunc),
    /*  29 */ SyscallDesc("pause", unimplementedFunc),
    /*  30 */ SyscallDesc("utime", unimplementedFunc),
    /*  31 */ SyscallDesc("stty", unimplementedFunc),
    /*  32 */ SyscallDesc("gtty", unimplementedFunc),
    /*  33 */ SyscallDesc("access", unimplementedFunc),
    /*  34 */ SyscallDesc("nice", unimplementedFunc),
    /*  35 */ SyscallDesc("ftime", unimplementedFunc),
    /*  36 */ SyscallDesc("sync", unimplementedFunc),
    /*  37 */ SyscallDesc("kill", unimplementedFunc),
    /*  38 */ SyscallDesc("rename", unimplementedFunc),
    /*  39 */ SyscallDesc("mkdir", unimplementedFunc),
    /*  40 */ SyscallDesc("rmdir", unimplementedFunc),
    /*  41 */ SyscallDesc("dup", unimplementedFunc),
    /*  42 */ SyscallDesc("pipe", unimplementedFunc),
    /*  43 */ SyscallDesc("times", unimplementedFunc),
    /*  44 */ SyscallDesc("prof", unimplementedFunc),
    /*  45 */ SyscallDesc("brk", brkFunc),
    /*  46 */ SyscallDesc("setgid", unimplementedFunc),
    /*  47 */ SyscallDesc("getgid", unimplementedFunc),
    /*  48 */ SyscallDesc("signal", unimplementedFunc),
    /*  49 */ SyscallDesc("geteuid", unimplementedFunc),
    /*  50 */ SyscallDesc("getegid", unimplementedFunc),
    /*  51 */ SyscallDesc("acct", unimplementedFunc),
    /*  52 */ SyscallDesc("umount2", unimplementedFunc),
    /*  53 */ SyscallDesc("lock", unimplementedFunc),
    /*  54 */ SyscallDesc("ioctl", unimplementedFunc),
    /*  55 */ SyscallDesc("fcntl", unimplementedFunc),
    /*  56 */ SyscallDesc("mpx", unimplementedFunc),
    /*  57 */ SyscallDesc("setpgid", unimplementedFunc),
    /*  58 */ SyscallDesc("ulimit", unimplementedFunc),
    /*  59 */ SyscallDesc("oldolduname", unimplementedFunc),
    /*  60 */ SyscallDesc("umask", unimplementedFunc),
    /*  61 */ SyscallDesc("chroot", unimplementedFunc),
    /*  62 */ SyscallDesc("ustat", unimplementedFunc),
    /*  63 */ SyscallDesc("dup2", unimplementedFunc),
    /*  64 */ SyscallDesc("getppid", unimplementedFunc),
    /*  65 */ SyscallDesc("getpgrp", unimplementedFunc),
    /*  66 */ SyscallDesc("setsid", unimplementedFunc),
    /*  67 */ SyscallDesc("sigaction", unimplementedFunc),
    /*  68 */ SyscallDesc("sgetmask", unimplementedFunc),
    /*  69 */ SyscallDesc("ssetmask", unimplementedFunc),
    /*  70 */ SyscallDesc("setreuid", unimplementedFunc),
    /*  71 */ SyscallDesc("setregid", unimplementedFunc),
    /*  72 */ SyscallDesc("sigsuspend", unimplementedFunc),
    /*  73 */ SyscallDesc("sigpending", unimplementedFunc),
    /*  74 */ SyscallDesc("sethostname", unimplementedFunc),
    /*  75 */ SyscallDesc("setrlimit", unimplementedFunc),
    /*  76 */ SyscallDesc("getrlimit", unimplementedFunc),
    /*  77 */ SyscallDesc("getrusage", unimplementedFunc),
    /*  78 */ SyscallDesc("gettimeofday", unimplementedFunc),
    /*  79 */ SyscallDesc("settimeofday", unimplementedFunc),
    /*  80 */ SyscallDesc("getgroups", unimplementedFunc),
    /*  81 */ SyscallDesc("setgroups", unimplementedFunc),
    /*  82 */ SyscallDesc("select", unimplementedFunc),
    /*  83 */ SyscallDesc("symlink", unimplementedFunc),
    /*  84 */ SyscallDesc("oldlstat", unimplementedFunc),
    /*  85 */ SyscallDesc("readlink", unimplementedFunc),
    /*  86 */ SyscallDesc("uselib", unimplementedFunc),
    /*  87 */ SyscallDesc("swapon", unimplementedFunc),
    /*  88 */ SyscallDesc("reboot", unimplementedFunc),
    /*  89 */ SyscallDesc("readdir", unimplementedFunc),
    /*  90 */ SyscallDesc("mmap", unimplementedFunc),
    /*  91 */ SyscallDesc("munmap", unimplementedFunc),
    /*  92 */ SyscallDesc("truncate", unimplementedFunc),
    /*  93 */ SyscallDesc("ftruncate", unimplementedFunc),
    /*  94 */ SyscallDesc("fchmod", unimplementedFunc),
    /*  95 */ SyscallDesc("fchown", unimplementedFunc),
    /*  96 */ SyscallDesc("getpriority", unimplementedFunc),
    /*  97 */ SyscallDesc("setpriority", unimplementedFunc),
    /*  98 */ SyscallDesc("profil", unimplementedFunc),
    /*  99 */ SyscallDesc("statfs", unimplementedFunc),
    /* 100 */ SyscallDesc("fstatfs", unimplementedFunc),
    /* 101 */ SyscallDesc("ioperm", unimplementedFunc),
    /* 102 */ SyscallDesc("socketcall", unimplementedFunc),
    /* 103 */ SyscallDesc("syslog", unimplementedFunc),
    /* 104 */ SyscallDesc("setitimer", unimplementedFunc),
    /* 105 */ SyscallDesc("getitimer", unimplementedFunc),
    /* 106 */ SyscallDesc("stat", unimplementedFunc),
    /* 107 */ SyscallDesc("lstat", unimplementedFunc),
    /* 108 */ SyscallDesc("fstat", unimplementedFunc),
    /* 109 */ SyscallDesc("olduname", unimplementedFunc),
    /* 110 */ SyscallDesc("iopl", unimplementedFunc),
    /* 111 */ SyscallDesc("vhangup", unimplementedFunc),
    /* 112 */ SyscallDesc("idle", unimplementedFunc),
    /* 113 */ SyscallDesc("vm86old", unimplementedFunc),
    /* 114 */ SyscallDesc("wait4", unimplementedFunc),
    /* 115 */ SyscallDesc("swapoff", unimplementedFunc),
    /* 116 */ SyscallDesc("sysinfo", unimplementedFunc),
    /* 117 */ SyscallDesc("ipc", unimplementedFunc),
    /* 118 */ SyscallDesc("fsync", unimplementedFunc),
    /* 119 */ SyscallDesc("sigreturn", unimplementedFunc),
    /* 120 */ SyscallDesc("clone", unimplementedFunc),
    /* 121 */ SyscallDesc("setdomainname", unimplementedFunc),
    /* 122 */ SyscallDesc("uname", unameFunc),
    /* 123 */ SyscallDesc("modify_ldt", unimplementedFunc),
    /* 124 */ SyscallDesc("adjtimex", unimplementedFunc),
    /* 125 */ SyscallDesc("mprotect", unimplementedFunc),
    /* 126 */ SyscallDesc("sigprocmask", unimplementedFunc),
    /* 127 */ SyscallDesc("create_module", unimplementedFunc),
    /* 128 */ SyscallDesc("init_module", unimplementedFunc),
    /* 129 */ SyscallDesc("delete_module", unimplementedFunc),
    /* 130 */ SyscallDesc("get_kernel_syms", unimplementedFunc),
    /* 131 */ SyscallDesc("quotactl", unimplementedFunc),
    /* 132 */ SyscallDesc("getpgid", unimplementedFunc),
    /* 133 */ SyscallDesc("fchdir", unimplementedFunc),
    /* 134 */ SyscallDesc("bdflush", unimplementedFunc),
    /* 135 */ SyscallDesc("sysfs", unimplementedFunc),
    /* 136 */ SyscallDesc("personality", unimplementedFunc),
    /* 137 */ SyscallDesc("afs_syscall", unimplementedFunc),
    /* 138 */ SyscallDesc("setfsuid", unimplementedFunc),
    /* 139 */ SyscallDesc("setfsgid", unimplementedFunc),
    /* 140 */ SyscallDesc("_llseek", unimplementedFunc),
    /* 141 */ SyscallDesc("getdents", unimplementedFunc),
    /* 142 */ SyscallDesc("_newselect", unimplementedFunc),
    /* 143 */ SyscallDesc("flock", unimplementedFunc),
    /* 144 */ SyscallDesc("msync", unimplementedFunc),
    /* 145 */ SyscallDesc("readv", unimplementedFunc),
    /* 146 */ SyscallDesc("writev", writevFunc<X86Linux32>),
    /* 147 */ SyscallDesc("getsid", unimplementedFunc),
    /* 148 */ SyscallDesc("fdatasync", unimplementedFunc),
    /* 149 */ SyscallDesc("_sysctl", unimplementedFunc),
    /* 150 */ SyscallDesc("mlock", unimplementedFunc),
    /* 151 */ SyscallDesc("munlock", unimplementedFunc),
    /* 152 */ SyscallDesc("mlockall", unimplementedFunc),
    /* 153 */ SyscallDesc("munlockall", unimplementedFunc),
    /* 154 */ SyscallDesc("sched_setparam", unimplementedFunc),
    /* 155 */ SyscallDesc("sched_getparam", unimplementedFunc),
    /* 156 */ SyscallDesc("sched_setscheduler", unimplementedFunc),
    /* 157 */ SyscallDesc("sched_getscheduler", unimplementedFunc),
    /* 158 */ SyscallDesc("sched_yield", unimplementedFunc),
    /* 159 */ SyscallDesc("sched_get_priority_max", unimplementedFunc),
    /* 160 */ SyscallDesc("sched_get_priority_min", unimplementedFunc),
    /* 161 */ SyscallDesc("sched_rr_get_interval", unimplementedFunc),
    /* 162 */ SyscallDesc("nanosleep", unimplementedFunc),
    /* 163 */ SyscallDesc("mremap", unimplementedFunc),
    /* 164 */ SyscallDesc("setresuid", unimplementedFunc),
    /* 165 */ SyscallDesc("getresuid", unimplementedFunc),
    /* 166 */ SyscallDesc("vm86", unimplementedFunc),
    /* 167 */ SyscallDesc("query_module", unimplementedFunc),
    /* 168 */ SyscallDesc("poll", unimplementedFunc),
    /* 169 */ SyscallDesc("nfsservctl", unimplementedFunc),
    /* 170 */ SyscallDesc("setresgid", unimplementedFunc),
    /* 171 */ SyscallDesc("getresgid", unimplementedFunc),
    /* 172 */ SyscallDesc("prctl", unimplementedFunc),
    /* 173 */ SyscallDesc("rt_sigreturn", unimplementedFunc),
    /* 174 */ SyscallDesc("rt_sigaction", unimplementedFunc),
    /* 175 */ SyscallDesc("rt_sigprocmask", unimplementedFunc),
    /* 176 */ SyscallDesc("rt_sigpending", unimplementedFunc),
    /* 177 */ SyscallDesc("rt_sigtimedwait", unimplementedFunc),
    /* 178 */ SyscallDesc("rt_sigqueueinfo", unimplementedFunc),
    /* 179 */ SyscallDesc("rt_sigsuspend", unimplementedFunc),
    /* 180 */ SyscallDesc("pread64", unimplementedFunc),
    /* 181 */ SyscallDesc("pwrite64", unimplementedFunc),
    /* 182 */ SyscallDesc("chown", unimplementedFunc),
    /* 183 */ SyscallDesc("getcwd", unimplementedFunc),
    /* 184 */ SyscallDesc("capget", unimplementedFunc),
    /* 185 */ SyscallDesc("capset", unimplementedFunc),
    /* 186 */ SyscallDesc("sigaltstack", unimplementedFunc),
    /* 187 */ SyscallDesc("sendfile", unimplementedFunc),
    /* 188 */ SyscallDesc("getpmsg", unimplementedFunc),
    /* 189 */ SyscallDesc("putpmsg", unimplementedFunc),
    /* 190 */ SyscallDesc("vfork", unimplementedFunc),
    /* 191 */ SyscallDesc("ugetrlimit", unimplementedFunc),
    /* 192 */ SyscallDesc("mmap2", unimplementedFunc),
    /* 193 */ SyscallDesc("truncate64", unimplementedFunc),
    /* 194 */ SyscallDesc("ftruncate64", unimplementedFunc),
    /* 195 */ SyscallDesc("stat64", unimplementedFunc),
    /* 196 */ SyscallDesc("lstat64", unimplementedFunc),
    /* 197 */ SyscallDesc("fstat64", unimplementedFunc),
    /* 198 */ SyscallDesc("lchown32", unimplementedFunc),
    /* 199 */ SyscallDesc("getuid32", unimplementedFunc),
    /* 200 */ SyscallDesc("getgid32", unimplementedFunc),
    /* 201 */ SyscallDesc("geteuid32", unimplementedFunc),
    /* 202 */ SyscallDesc("getegid32", unimplementedFunc),
    /* 203 */ SyscallDesc("setreuid32", unimplementedFunc),
    /* 204 */ SyscallDesc("setregid32", unimplementedFunc),
    /* 205 */ SyscallDesc("getgroups32", unimplementedFunc),
    /* 206 */ SyscallDesc("setgroups32", unimplementedFunc),
    /* 207 */ SyscallDesc("fchown32", unimplementedFunc),
    /* 208 */ SyscallDesc("setresuid32", unimplementedFunc),
    /* 209 */ SyscallDesc("getresuid32", unimplementedFunc),
    /* 210 */ SyscallDesc("setresgid32", unimplementedFunc),
    /* 211 */ SyscallDesc("getresgid32", unimplementedFunc),
    /* 212 */ SyscallDesc("chown32", unimplementedFunc),
    /* 213 */ SyscallDesc("setuid32", unimplementedFunc),
    /* 214 */ SyscallDesc("setgid32", unimplementedFunc),
    /* 215 */ SyscallDesc("setfsuid32", unimplementedFunc),
    /* 216 */ SyscallDesc("setfsgid32", unimplementedFunc),
    /* 217 */ SyscallDesc("pivot_root", unimplementedFunc),
    /* 218 */ SyscallDesc("mincore", unimplementedFunc),
    /* 219 */ SyscallDesc("madvise", unimplementedFunc),
    /* 220 */ SyscallDesc("madvise1", unimplementedFunc),
    /* 221 */ SyscallDesc("getdents64", unimplementedFunc),
    /* 222 */ SyscallDesc("fcntl64", unimplementedFunc),
    /* 223 */ SyscallDesc("unused", unimplementedFunc),
    /* 224 */ SyscallDesc("gettid", unimplementedFunc),
    /* 225 */ SyscallDesc("readahead", unimplementedFunc),
    /* 226 */ SyscallDesc("setxattr", unimplementedFunc),
    /* 227 */ SyscallDesc("lsetxattr", unimplementedFunc),
    /* 228 */ SyscallDesc("fsetxattr", unimplementedFunc),
    /* 229 */ SyscallDesc("getxattr", unimplementedFunc),
    /* 230 */ SyscallDesc("lgetxattr", unimplementedFunc),
    /* 231 */ SyscallDesc("fgetxattr", unimplementedFunc),
    /* 232 */ SyscallDesc("listxattr", unimplementedFunc),
    /* 233 */ SyscallDesc("llistxattr", unimplementedFunc),
    /* 234 */ SyscallDesc("flistxattr", unimplementedFunc),
    /* 235 */ SyscallDesc("removexattr", unimplementedFunc),
    /* 236 */ SyscallDesc("lremovexattr", unimplementedFunc),
    /* 237 */ SyscallDesc("fremovexattr", unimplementedFunc),
    /* 238 */ SyscallDesc("tkill", unimplementedFunc),
    /* 239 */ SyscallDesc("sendfile64", unimplementedFunc),
    /* 240 */ SyscallDesc("futex", unimplementedFunc),
    /* 241 */ SyscallDesc("sched_setaffinity", unimplementedFunc),
    /* 242 */ SyscallDesc("sched_getaffinity", unimplementedFunc),
    /* 243 */ SyscallDesc("set_thread_area", unimplementedFunc),
    /* 244 */ SyscallDesc("get_thread_area", unimplementedFunc),
    /* 245 */ SyscallDesc("io_setup", unimplementedFunc),
    /* 246 */ SyscallDesc("io_destroy", unimplementedFunc),
    /* 247 */ SyscallDesc("io_getevents", unimplementedFunc),
    /* 248 */ SyscallDesc("io_submit", unimplementedFunc),
    /* 249 */ SyscallDesc("io_cancel", unimplementedFunc),
    /* 250 */ SyscallDesc("fadvise64", unimplementedFunc),
    /* 251 */ SyscallDesc("unused", unimplementedFunc),
    /* 252 */ SyscallDesc("exit_group", unimplementedFunc),
    /* 253 */ SyscallDesc("lookup_dcookie", unimplementedFunc),
    /* 254 */ SyscallDesc("epoll_create", unimplementedFunc),
    /* 255 */ SyscallDesc("epoll_ctl", unimplementedFunc),
    /* 256 */ SyscallDesc("epoll_wait", unimplementedFunc),
    /* 257 */ SyscallDesc("remap_file_pages", unimplementedFunc),
    /* 258 */ SyscallDesc("set_tid_address", unimplementedFunc),
    /* 259 */ SyscallDesc("timer_create", unimplementedFunc),
    /* 260 */ SyscallDesc("timer_settime", unimplementedFunc),
    /* 261 */ SyscallDesc("timer_gettime", unimplementedFunc),
    /* 262 */ SyscallDesc("timer_getoverrun", unimplementedFunc),
    /* 263 */ SyscallDesc("timer_delete", unimplementedFunc),
    /* 264 */ SyscallDesc("clock_settime", unimplementedFunc),
    /* 265 */ SyscallDesc("clock_gettime", unimplementedFunc),
    /* 266 */ SyscallDesc("clock_getres", unimplementedFunc),
    /* 267 */ SyscallDesc("clock_nanosleep", unimplementedFunc),
    /* 268 */ SyscallDesc("statfs64", unimplementedFunc),
    /* 269 */ SyscallDesc("fstatfs64", unimplementedFunc),
    /* 270 */ SyscallDesc("tgkill", unimplementedFunc),
    /* 271 */ SyscallDesc("utimes", unimplementedFunc),
    /* 272 */ SyscallDesc("fadvise64_64", unimplementedFunc),
    /* 273 */ SyscallDesc("vserver", unimplementedFunc),
    /* 274 */ SyscallDesc("mbind", unimplementedFunc),
    /* 275 */ SyscallDesc("get_mempolicy", unimplementedFunc),
    /* 276 */ SyscallDesc("set_mempolicy", unimplementedFunc),
    /* 277 */ SyscallDesc("mq_open", unimplementedFunc),
    /* 278 */ SyscallDesc("mq_unlink", unimplementedFunc),
    /* 279 */ SyscallDesc("mq_timedsend", unimplementedFunc),
    /* 280 */ SyscallDesc("mq_timedreceive", unimplementedFunc),
    /* 281 */ SyscallDesc("mq_notify", unimplementedFunc),
    /* 282 */ SyscallDesc("mq_getsetattr", unimplementedFunc),
    /* 283 */ SyscallDesc("kexec_load", unimplementedFunc),
    /* 284 */ SyscallDesc("waitid", unimplementedFunc),
    /* 285 */ SyscallDesc("sys_setaltroot", unimplementedFunc),
    /* 286 */ SyscallDesc("add_key", unimplementedFunc),
    /* 287 */ SyscallDesc("request_key", unimplementedFunc),
    /* 288 */ SyscallDesc("keyctl", unimplementedFunc),
    /* 289 */ SyscallDesc("ioprio_set", unimplementedFunc),
    /* 290 */ SyscallDesc("ioprio_get", unimplementedFunc),
    /* 291 */ SyscallDesc("inotify_init", unimplementedFunc),
    /* 292 */ SyscallDesc("inotify_add_watch", unimplementedFunc),
    /* 293 */ SyscallDesc("inotify_rm_watch", unimplementedFunc),
    /* 294 */ SyscallDesc("migrate_pages", unimplementedFunc),
    /* 295 */ SyscallDesc("openat", unimplementedFunc),
    /* 296 */ SyscallDesc("mkdirat", unimplementedFunc),
    /* 297 */ SyscallDesc("mknodat", unimplementedFunc),
    /* 298 */ SyscallDesc("fchownat", unimplementedFunc),
    /* 299 */ SyscallDesc("futimesat", unimplementedFunc),
    /* 300 */ SyscallDesc("fstatat64", unimplementedFunc),
    /* 301 */ SyscallDesc("unlinkat", unimplementedFunc),
    /* 302 */ SyscallDesc("renameat", unimplementedFunc),
    /* 303 */ SyscallDesc("linkat", unimplementedFunc),
    /* 304 */ SyscallDesc("symlinkat", unimplementedFunc),
    /* 305 */ SyscallDesc("readlinkat", unimplementedFunc),
    /* 306 */ SyscallDesc("fchmodat", unimplementedFunc),
    /* 307 */ SyscallDesc("faccessat", unimplementedFunc),
    /* 308 */ SyscallDesc("pselect6", unimplementedFunc),
    /* 309 */ SyscallDesc("ppoll", unimplementedFunc),
    /* 310 */ SyscallDesc("unshare", unimplementedFunc),
    /* 311 */ SyscallDesc("set_robust_list", unimplementedFunc),
    /* 312 */ SyscallDesc("get_robust_list", unimplementedFunc),
    /* 313 */ SyscallDesc("splice", unimplementedFunc),
    /* 314 */ SyscallDesc("sync_file_range", unimplementedFunc),
    /* 315 */ SyscallDesc("tee", unimplementedFunc),
    /* 316 */ SyscallDesc("vmsplice", unimplementedFunc),
    /* 317 */ SyscallDesc("move_pages", unimplementedFunc),
    /* 318 */ SyscallDesc("getcpu", unimplementedFunc),
    /* 319 */ SyscallDesc("epoll_pwait", unimplementedFunc),
    /* 320 */ SyscallDesc("utimensat", unimplementedFunc),
    /* 321 */ SyscallDesc("signalfd", unimplementedFunc),
    /* 322 */ SyscallDesc("timerfd", unimplementedFunc),
    /* 323 */ SyscallDesc("eventfd", unimplementedFunc)
};
