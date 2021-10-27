
#!/bin/bash

var=\"$1\"

analyzeTDS --f $1 && eval "TDSRoot '$TDS_PATH/macros/savespc.C($var)'"


