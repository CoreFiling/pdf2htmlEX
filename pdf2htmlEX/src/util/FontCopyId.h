
#ifndef FONT_COPY_ID_H__
#define FONT_COPY_ID_H__

typedef struct FontCopyId {
  long long font_id;
  uint8_t copy;

  FontCopyId(long long font_id, uint8_t copy) : font_id(font_id), copy(copy) { }
  inline bool operator==(const FontCopyId &other) const
  {
    return font_id == other.font_id && copy == other.copy;
  }
} FontCopyId;

template <>
struct std::hash<FontCopyId>
{
  inline std::size_t operator()(const FontCopyId& k) const
  {
    using std::size_t;
    using std::hash;
    return hash<long long>()(k.font_id) ^ (hash<uint8_t>()(k.copy) << 1);
  }
};

#endif