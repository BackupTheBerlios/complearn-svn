#include "complearn/real-compressor.h"

struct _CompLearnRealCompressor {
};

gboolean real_compressor_is_operational(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->is_operational(rc);
}

gboolean real_compressor_is_hash_function(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->is_hash_function(rc);
}

gboolean real_compressor_is_just_size(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->is_just_size(rc);
}

gdouble real_compressor_compressed_size(CompLearnRealCompressor *rc,const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->compressed_size(rc, input);
}

GString *real_compressor_hash(CompLearnRealCompressor *rc,const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->hash(rc, input);
}

GString *real_compressor_compress(CompLearnRealCompressor *rc,const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->compress(rc, input);
}

GString *real_compressor_decompress(CompLearnRealCompressor *rc,const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->decompress(rc, input);
}

GString *real_compressor_blurb(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> blurb(rc);
}

GString *real_compressor_name(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> name(rc);
}

GString *real_compressor_compressor_version(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->
                         compressor_version(rc);
}

GString *real_compressor_binding_version(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)->
                         binding_version(rc);
}

GString *real_compressor_canonical_extension(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> canonical_extension(rc);
}

gboolean real_compressor_is_compressible(CompLearnRealCompressor *rc,
                                           const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> is_compressible(rc, input);
}

gboolean real_compressor_is_decompressible(CompLearnRealCompressor *rc,
                                           const GString *input)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> is_decompressible(rc, input);
}

guint64 real_compressor_window_size(CompLearnRealCompressor *rc)
{
  return COMPLEARN_REAL_COMPRESSOR_GET_INTERFACE(rc)-> window_size(rc);
}


void
real_compressor_interface_init (gpointer         g_iface,
                    gpointer         iface_data)
{
  CompLearnRealCompressorIface *iface = (CompLearnRealCompressorIface *)g_iface;
  iface->compress = real_compressor_compress;
  iface->decompress = real_compressor_decompress;
  iface->blurb = real_compressor_blurb;
  iface->name = real_compressor_name;
  iface->compressor_version = real_compressor_compressor_version;
  iface->binding_version = real_compressor_binding_version;
  iface->is_compressible = real_compressor_is_compressible;
  iface->is_decompressible = real_compressor_is_decompressible;
  iface->window_size = real_compressor_window_size;
  iface->is_just_size = real_compressor_is_just_size;
  iface->is_hash_function = real_compressor_is_hash_function;
  iface->is_operational = real_compressor_is_operational;
  iface->compressed_size = real_compressor_compressed_size;
  iface->canonical_extension = real_compressor_canonical_extension;
}

GType real_compressor_get_type (void)
{
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (CompLearnRealCompressorIface),
      NULL,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      sizeof (CompLearnRealCompressor),
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };
    static const GInterfaceInfo rc_info = {
      (GInterfaceInitFunc) real_compressor_interface_init, /* interface_init */
      NULL,               /* interface_finalize */
      NULL          /* interface_data */
    };
    type = g_type_register_static (G_TYPE_INTERFACE,
                                   "CompLearnRealCompressorType",
                                   &info, 0);
  }
  return type;
}
