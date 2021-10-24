
#!/bin/bash

analyzeTDS --f $1
var=\"$1\"
eval "TDSRoot '$TDS_PATH/macros/savespc.C($var)'"


