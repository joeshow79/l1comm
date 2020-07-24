#!/usr/bin/env python3

import base64
import sys

img = open(sys.argv[1], 'rb')

base_out = base64.b64encode(img.read())

img.close()

print(base_out)

