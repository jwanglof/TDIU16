SRCDIR = ..

# Test programs to compile, and a list of sources for each.
# To add a new test, put its name on the PROGS list
# and then add a name_SRC line that lists its source files.
PROGS = cat cmp cp echo halt hex-dump ls mcat mcp mkdir pwd rm shell \
	bubsort insult lineup matmult recursor \
	sumargv pfs pfs_reader pfs_writer dummy longrun \
	child parent generic_parent longrun_interactive busy \
	line_echo file_syscall_tests longrun_nowait shellcode \
	crack overflow dir_stress create_file create_remove_file

# Added test programs
sumargv_SRC = sumargv.c
pfs_SRC = pfs.c
pfs_reader_SRC = pfs_reader.c
pfs_writer_SRC = pfs_writer.c
longrun_SRC = longrun.c
longrun_nowait_SRC = longrun_nowait.c
longrun_interactive_SRC = longrun_interactive.c
dummy_SRC = dummy.c
busy_SRC = busy.c
child_SRC = child.c
parent_SRC = parent.c
generic_parent_SRC = generic_parent.c
line_echo_SRC = line_echo.c
file_syscall_tests_SRC = file_syscall_tests.c
shellcode_SRC = shellcode.c
crack_SRC = crack.c
overflow_SRC = overflow.c
dir_stress_SRC = dir_stress.c
create_file_SRC = create_file.c
create_remove_file_SRC = create_remove_file.c

# Should work from project 2 onward.
cat_SRC = cat.c
cmp_SRC = cmp.c
cp_SRC = cp.c
echo_SRC = echo.c
halt_SRC = halt.c
hex-dump_SRC = hex-dump.c
insult_SRC = insult.c
lineup_SRC = lineup.c
ls_SRC = ls.c
recursor_SRC = recursor.c
rm_SRC = rm.c

# Should work in project 3; also in project 4 if VM is included.
bubsort_SRC = bubsort.c
matmult_SRC = matmult.c
mcat_SRC = mcat.c
mcp_SRC = mcp.c

# Should work in project 4.
mkdir_SRC = mkdir.c
pwd_SRC = pwd.c
shell_SRC = shell.c

include $(SRCDIR)/Make.config
include $(SRCDIR)/Makefile.userprog
