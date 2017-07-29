#ifndef SYB_NOT_INLINE_H
#define SYB_NOT_INLINE_H
//TODO: HACK: remove

#ifdef NOT_INLINES
namespace qx {
  namespace syb {
    template<typename FunT, typename T>
    void notInline(FunT const& f, T& v) {
        f.runInline(v);
    }
  }
}
#endif

#endif
