s/\([^;]*\);/\1\/\//
s/^\.if/#if/
s/^\.else/#else/
s/^\.endif/#endif/
s/^\.equ\([ 	][^,]*\),/#define\1/
