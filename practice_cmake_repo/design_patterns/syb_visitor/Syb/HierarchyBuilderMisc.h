/// \brief Just a helper functions for generation acceptor's interfaces
#ifndef HIERARCHY_BUILDER_MISC_H
#define HIERARCHY_BUILDER_MISC_H

#include "boost/mpl/find.hpp"
#include "boost/mpl/size.hpp"
#include "boost/utility/enable_if.hpp"

namespace qx {
  namespace syb {

    namespace mpl = boost::mpl;

    struct HierarchyItemTag{
      virtual ~HierarchyItemTag();
      virtual void test() {}
    };

    template <class H>
		struct GetSubHierarchy;

		template <
      template <class, class, class> class H,
      class ParentT, class Base, class T
    >
		struct GetSubHierarchy<H<ParentT, Base, T> >
		{
			typedef Base type;
		};

		template <
      template <class, class, class> class H,
      class ParentT, class T
    >
		struct GetSubHierarchy<H<ParentT, HierarchyItemTag, T> >
		{
			typedef HierarchyItemTag type;
		};

		template <class H, int N, class EnableT = void>
		struct GetSubHierarchyDeep;

		template <
      template <class, class, class> class H,
      class ParentT, class Base, class T, int depth
    >
		struct GetSubHierarchyDeep <
        H<ParentT, Base, T>,
        depth,
        typename boost::enable_if_c< (depth >= 0) >::type
      >
		{
			typedef typename GetSubHierarchyDeep<
				typename GetSubHierarchy <
          H<ParentT, Base, T> >::type,
				  depth - 1
			  >::type type;
		};

		template <
      template <class, class, class> class H,
      class ParentT, class Base, class T
    >
		struct GetSubHierarchyDeep<H<ParentT, Base, T>, 0>
		{
			typedef H<ParentT, Base, T> type;
		};

		template <class H, class Sequence, class T>
		struct GetSubHierarchyByType
		{
			typedef typename GetSubHierarchyDeep
      <
				H,
        (mpl::size<Sequence>::type::value - 1) - mpl::find<Sequence, T>::type::pos::value
			>::type type;
		};
  }
}


#endif /*HIERARCHY_BUILDER_MISC_H*/
