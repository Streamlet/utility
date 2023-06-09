import os, sys

cmd = 'go build %s' % ' '.join(sys.argv[1:])
print(cmd)
os.system(cmd)
