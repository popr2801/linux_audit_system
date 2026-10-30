# linux_audit

A small, dependency-free **Linux audit daemon in C**. It runs in the background and logs process starts/stops, root login sessions and a system snapshot, using only libc and read-only access to `/proc`, `passwd` and `utmp`.

## Features

| Feature | Module | What it does |
|---|---|---|
| Process monitoring | `processes.c` | Scans `/proc/<pid>/status` every 10 s and logs started/terminated processes with PID, name and owner |
| Root session tracking | `sessions.c`, `users.c` | Reads `utmp`, resolves users to UIDs, logs root sessions (user, TTY, host) only when they change |
| System snapshot | `system_info.c` | Logs hostname, kernel, architecture and memory usage at start-up |
| Daemon lifecycle | `daemon.c`, `main.c` | Double fork, `setsid`, single-instance check, clean shutdown on SIGTERM/SIGINT |

Example log:

```text
=== Audit started [2026-10-30 09:21:40] ===
Hostname: vm
Kernel: 6.18.44-fc-v185 (#1 SMP PREEMPT_DYNAMIC @0)
Architecture: x86_64
Memory: 3995 MB total, 5.45% used
-----------------------------------------
[2026-10-30 09:21:50] Process started: PID 206 (sleep), UID 0 (root)
[2026-10-30 09:21:50] Process terminated: PID 200 (sleep)
Audit daemon stopped
```

## Build and run

Requires Linux, glibc, `make` and GCC/Clang.

```bash
make          # production build -> ./linux_audit  (log: /var/log, PID: /run)
make dev      # logs to /tmp, no root needed -> ./linux_audit_dev
make debug    # dev paths + AddressSanitizer/UBSan
make clean

sudo ./linux_audit
tail -f /var/log/linux_audit.log
sudo kill "$(cat /run/linux_audit.pid)"    # clean shutdown
```

Start-up errors are sent to syslog (`journalctl -t linux_audit`). Paths can be overridden with `-DLOG_PATH=...` and `-DPID_PATH=...`.

## Architecture

```text
 /proc/meminfo, uname() --> system_info.c --+
 /proc/<pid>/status ------> processes.c ----+
 utmp (getutent) ---------> sessions.c -----+--> main.c (10 s loop) --> log file (0600)
 passwd / NSS ------------> users.c --------+     + daemon.c            PID file
                                                                        syslog (errors)
```

All system data is treated as untrusted: collection modules validate it and copy it into fixed-size structures before the main loop uses it.

## Security design

- **Minimal attack surface:** no sockets, no `exec`/`system()`, no third-party code.
- **Memory safety by construction:** static fixed-size buffers, no `malloc`, bounded copies with `snprintf`, records zeroed before use.
- **Safe file handling:** log and PID file opened with `O_NOFOLLOW | O_CLOEXEC`, mode `0600`, `umask(077)`, stored in `/var/log` and `/run` instead of `/tmp`.
- **Correct audit data:** unresolved users get `UNKNOWN_UID`, so they can never be mistaken for root.
- **Robust lifecycle:** single-instance check, async-signal-safe shutdown handlers, errors reported to syslog after detaching.
- **Defensive parsing:** typed `sscanf` with success checks, NUL-safe reads of fixed-width `utmp` fields, vanished processes discarded.

## Code review findings (all fixed)

| Severity | Finding |
|---|---|
| High | Log/PID files in `/tmp` with no symlink protection |
| High | `endpwent()` missing: user list empty on 2nd call, so every session was reported as root |
| Medium | Off-by-one write in `load_users` |
| Medium | `size_t` compared with `< 0`: failed `/proc` read led to a huge `memcpy` |
| Medium | `log == NULL` checked instead of `*log == NULL` |
| Medium | `utmp` fields read as NUL-terminated strings |
| Low | `strcpy` of a 65-byte value into a 64-byte buffer; process names cut at the first space; stale struct fields |
| Info | `daemon.h` not self-contained |

Verified: zero warnings with `-Wall -Wextra -Wpedantic`; repeatable and bounded user loading; clean SIGTERM/SIGINT shutdown with PID-file removal; second instance refused. Not yet verified: behaviour with real root sessions, more than 4096 processes, captured sanitizer output.

## Limitations

- Polling every 10 s: processes living shorter than that, or PIDs reused within one interval, are missed.
- At most 4096 processes are tracked (warning sent to syslog beyond that).
- Control characters in process names or hosts are logged unescaped (log injection).
- No log rotation; the log is not tamper-evident and lives on the monitored host.
- The PID-file check is not atomic; `utmp` may be empty on systemd-only systems and containers.

## Roadmap

| Priority | Improvement |
|---|---|
| P1 | Escape non-printable bytes in logged names and hosts |
| P1 | Log rotation (reopen on SIGHUP) and optional size limit |
| P1 | Atomic single-instance lock with `flock()` |
| P1 | Hardening flags: `-fstack-protector-strong`, `-D_FORTIFY_SOURCE=2`, `-fPIE -pie`, `-Wl,-z,relro,-z,now` |
| P2 | Event-driven monitoring via the netlink process connector |
| P2 | PID-reuse detection using process start time |
| P2 | Systemd unit with `NoNewPrivileges`/`ProtectSystem`, seccomp filter |
| P2 | systemd-logind sessions and failed-login (`btmp`) monitoring |
| P3 | Hash-chained logs and remote forwarding |
| P3 | Executable path and command line capture |
| P3 | Hash-set diffing without the 4096 cap |
| P3 | Config file, JSON output, UTC timestamps |
| P3 | Unit tests, parser fuzzing, sanitizer builds in CI |

`linux_audit` is a lightweight monitor and a readable reference for Linux daemon and `/proc` programming. For tamper-resistant, kernel-level auditing, use it alongside `auditd` or eBPF-based tools.

## License

Add a `LICENSE` file (for example MIT) before publishing.
