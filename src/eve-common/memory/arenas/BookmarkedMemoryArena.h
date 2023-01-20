#pragma once

#include <vector>

#include "StaticMemoryArena.h"

/**
 * Bookmarkable arena based off StaticPythonArena that allows
 * for easy and quick freeing
 */
template <class T = MemoryArena>
class BookmarkedMemoryArena : public StaticMemoryArena<T> {
public:
    typedef uint64_t BookmarkID;

    /**
     * @param reservedSize The byte amount to reserve
     */
    explicit BookmarkedMemoryArena (size_t reservedSize) :
        StaticMemoryArena<T> (reservedSize),
        mBookmarks (),
        mCurrentId (0) {}

    /**
     * Adds a bookmark to the list
     */
    [[nodiscard]] BookmarkID bookmark () {
        BookmarkInfo info {.id = mCurrentId++, .offset = this->mCurrentOffset};

        // keep track of current position
        mBookmarks.push_back (info);

        return info.id;
    }

    /**
     * Rewinds the arena to the given bookmark ID, zeroing any data after it
     *
     * @param[in] bookmarkId The bookmark to rewind to
     */
    void rewind (BookmarkID bookmarkId) {
        // search for the given bookmark if it exists
        iterator it = std::find_if(mBookmarks.begin(), mBookmarks.end(), [bookmarkId](BookmarkInfo current) {
            return current.id == bookmarkId;
        });

        if (it == mBookmarks.end()) {
            throw std::bad_alloc ();
        }

        this->mCurrentOffset = it->offset;

        // remove any bookmarks that are newer
        std::erase_if(mBookmarks, [bookmarkId](BookmarkInfo current) {
            return current.id > bookmarkId;
        });
    }

protected:
    BookmarkID mCurrentId;
    struct BookmarkInfo {
        BookmarkID id;
        size_t offset;
    };

    typedef std::vector<BookmarkInfo> storage_type;
    typedef storage_type::iterator  iterator;

    std::vector<BookmarkInfo> mBookmarks;
};
