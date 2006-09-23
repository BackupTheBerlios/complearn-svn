/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __COMPLEARN_H
#define __COMPLEARN_H

#include <assert.h>

#include <complearn/cltypes.h>
#include <complearn/uclconfig.h>

#include <complearn/adjadaptor.h>
#include <complearn/adjlist.h>
#include <complearn/adjmatrix.h>
#include <complearn/clerror.h>
#include <complearn/unrootedbinary.h>
#include <complearn/clnodeset.h>
#include <complearn/incrdm.h>
#include <complearn/envmap.h>
#include <complearn/stringstack.h>
#include <complearn/transadaptor.h>
#include <complearn/tastack.h>
#include <complearn/builtin.h>
#include <complearn/cloutput.h>
#include <complearn/datablock.h>
#include <complearn/datetime.h>
#include <complearn/datablockenum.h>
#include <complearn/dbe-array.h>
#include <complearn/dbe-dir.h>
#include <complearn/dbe-factory.h>
#include <complearn/dbe-filelist.h>
#include <complearn/dbe-singleton.h>
#include <complearn/dbe-stringlist.h>
#include <complearn/dbe-windowed.h>
#include <complearn/dlextcomp.h>
#include <complearn/dra.h>
#include <complearn/environment.h>
#include <complearn/clalloc.h>
#include <complearn/gdbmhelper.h>
#include <complearn/google.h>
#include <complearn/googlecache.h>
#include <complearn/labelperm.h>
#include <complearn/labelmaker.h>
#include <complearn/matfact.h>
#include <complearn/mutate.h>
#include <complearn/ncd-calc.h>
#include <complearn/paramlist.h>
#include <complearn/pathkeeper.h>
#include <complearn/quartet.h>
#include <complearn/rootedbinary.h>
#include <complearn/springball.h>
#include <complearn/tagfile.h>
#include <complearn/treeadaptor.h>
#include <complearn/treeholder.h>
#include <complearn/treeblaster.h>
#include <complearn/treemaster.h>
#include <complearn/dotparser.h>
#include <complearn/treemolder.h>
#include <complearn/treescore.h>
#include <complearn/treeparser.h>
#include <complearn/clog.h>
#include <complearn/clwrap.h>
#include <complearn/clparser.h>
#include <complearn/ncabz2.h>
#include <complearn/ncbase.h>
#include <complearn/ncazlib.h>

#ifdef HAVE_DMALLOC
#include "dmalloc.h"
#endif

#endif
