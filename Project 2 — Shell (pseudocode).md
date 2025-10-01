Project 2 — Shell (pseudocode)
Author: Nahom Walelinge
Date: 2025-09-30

MAIN:
  initialize shell state:
    - load environment variables (PATH)
    - initialize job list for background jobs
    - install SIGCHLD handler to reap children (optional)
  loop forever:
    - print prompt
    - read input line (use helper parser to split into tokens)
    - if EOF (Ctrl-D) -> exit shell
    - parse tokens for special characters: |  <  >  &
    - if built-in command (help, exit, pwd, cd, wait):
        call builtin handler
      else:
        create pipeline structure from tokens:
          - split tokens by '|' into commands[]
          - for each command, record argv[], infile (if < present), outfile (if > present)
          - detect background if last token is '&' (set background flag)
        execute pipeline:
          - if no pipes: fork child and run single command with redirection
          - else: create N-1 pipes, fork N children, for each child:
              * set up stdin/stdout via dup2 to appropriate pipe ends or to infile/outfile
              * close all unused pipe fds
              * do path resolution for argv[0]
              * execv(resolved_path, argv)
          - in parent:
              * close all pipe fds
              * if background: record pids in job list and do not wait
              * else: wait for all child pids to finish

BUILTINS:
  help: print help text
  exit: exit shell (optionally kill background jobs)
  pwd: print current working directory using getcwd
  cd [path]: chdir(path) (if no argument, go to HOME)
  wait: wait for all background jobs to finish (waitpid in a loop)

PATH RESOLUTION (for external commands):
  if argv[0] starts with '/': use it directly
  else:
    - get PATH environment variable (getenv("PATH"))
    - split PATH on ':' into dirs[]
    - for each dir in dirs:
        candidate = dir + "/" + argv[0]
        if access(candidate, X_OK) == 0:
          use this candidate
    - if none found: print error "command not found"

REDIRECTION (per single command):
  if infile specified:
    fd = open(infile, O_RDONLY)
    dup2(fd, STDIN_FILENO)
    close(fd)
  if outfile specified:
    fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644)
    dup2(fd, STDOUT_FILENO)
    close(fd)

PIPING (multiple commands):
  - create array pipes[n-1][2]
  - for i=0..n-1: fork child
      * if i > 0: dup2(pipes[i-1][0], STDIN_FILENO)
      * if i < n-1: dup2(pipes[i][1], STDOUT_FILENO)
      * close all pipe fds
      * apply redirections (if this command has explicit < or >, do it)
      * execv resolved command
  - parent: close all pipe fds
  - wait for children (unless background)

BACKGROUND JOBS:
  - if '&' detected (guaranteed last token): run pipeline but do not wait
  - store group of child pids in job table with job id
  - builtin wait: loop waitpid(-1, ...) until all background children finished

ERROR HANDLING:
  - check returns of fork, exec, open, dup2, pipe, chdir, getcwd
  - ensure all unused fds are closed to avoid hangs
  - print helpful diagnostics for user

END PSEUDOCODE
