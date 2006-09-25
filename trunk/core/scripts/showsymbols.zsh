#!/bin/sh
exec nm src/.libs/libcomplearn.a | grep ' T ' | cut -c12- | sort
