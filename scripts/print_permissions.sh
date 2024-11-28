#!/bin/bash

# path to the database
DB_PATH="../build/permissions.db"

# show table
sqlite3 $DB_PATH <<EOF
.mode column
.headers on
SELECT * FROM permissions;
EOF

