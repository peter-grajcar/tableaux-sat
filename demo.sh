#!/bin/bash
make

echo "==========Example 1=========="
echo "satisfiability of p & ~p"
build/tableaux-sat <<EOF
p & ~p
EOF

echo "==========Example 2=========="
echo "satisfiability of s"
echo "in theory q - p"
echo "          r - q"
echo "          (r - p) - s"
build/tableaux-sat -T <<EOF
s
q - p
r - q
(r - p) - s
EOF

echo "==========Example 3=========="
echo "dot language output example"
echo "satisfiability of p"
echo "in theory ~q"
echo "          p | q"
echo ""
build/tableaux-sat -T --dot <<EOF
p
~q
p | q
EOF

echo "==========Example 4=========="
echo "proof of a De Morgan's law"
echo "~(p | q) = (~p & ~q)"
build/tableaux-sat --proof <<EOF
~(p | q) = (~p & ~q)
EOF
echo "============================="
