# 
# The MIT License (MIT)
# 
# Copyright (c) 2015 Joel E. Merritt
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

LIB_OBJS=tiff_metadata.o
TIFF_METADATA_OBJS=main.o
TEST_OBJS=test.o
ALL_OBJS=$(LIB_OBJS) $(TIFF_METADATA_OBJS) $(TEST_OBJS)
H_SRCS=tiff_metadata.h
BINS=tiff_metadata test

CFLAGS=-Wall

tiff_metadata: $(TIFF_METADATA_OBJS) $(LIB_OBJS)
	$(CC) -o $@ $^

test: $(TEST_OBJS) $(LIB_OBJS)
	$(CC) -o $@ $^

.PHONY: all
all: $(BINS)

$(ALL_OBJS): $(H_SRCS)

.PHONY: clean
clean:
	$(RM) $(ALL_OBJS)
