#!/bin/bash

# path to the database
DB_PATH="../build/permissions.db"

# clear table
sqlite3 $DB_PATH <<EOF
DELETE FROM permissions;
EOF

echo "Table 'permissions' has been cleared."
