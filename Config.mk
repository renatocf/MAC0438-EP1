
# Project info
# ===============
PROJECT         := ep1-lucas-renato
VERSION         := 1.0.0
STD_NAMESPACE   := omnium
GIT_REMOTE_PATH := git@github.com:renatocf/MAC0438-EP1.git
MAINTEINER_NAME := Lucas Dário \
                   Renato Cordeiro Ferreira
MAINTEINER_MAIL := renatocf@ime.usp.br
SYNOPSIS        := First EP of MAC0438
DESCRIPTION     := Omnium run simulation

# Program settings
# ==================
BIN := Omnium

# Paths
# =======
# ASLIBS          := # Assembly paths
# CLIBS           := # C paths
# CXXLIBS         := # C++ paths
# LDLIBS          := # Linker paths

# Flags
# =======
CPPFLAGS        := -D_GNU_SOURCE
# ASFLAGS         := # Assembly Flags
CFLAGS          := -ansi -Wall -pedantic -O2 -g
# CXXFLAGS        := # C++ Flags
LDFLAGS         := -g

# Makeball list
# ===============
# 'include conf/makeball.mk' for pre-configured options
# to use the library 'makeball'
include conf/pthread.mk
