# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(sc-bad-write) begin
sc-bad-write: exit(-1)
EOF
pass;
