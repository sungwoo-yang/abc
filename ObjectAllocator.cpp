// Name       : Sungwoo Yang
// Assignment : ObjectAllocator
// Course     : CS280 Data Structures
// Term & Year: 2026 Spring

#include "ObjectAllocator.h"
#include <cstring>

/*!
  \brief Constructor.
  \param ObjectSize Size of each object.
  \param config Configuration settings.
*/
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) : PageList_(nullptr), FreeList_(nullptr), oaconfig(config)
{
    oastats.ObjectSize_ = ObjectSize;

    size_t HBlockSize = oaconfig.HBlockInfo_.size_;
    size_t BlockSize = HBlockSize + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

    if (oaconfig.Alignment_ > 1)
    {
        size_t overhead = sizeof(void *) + HBlockSize + oaconfig.PadBytes_;
        if (overhead % oaconfig.Alignment_ != 0)
        {
            oaconfig.LeftAlignSize_ = static_cast<unsigned>(oaconfig.Alignment_ - (overhead % oaconfig.Alignment_));
        }

        if (BlockSize % oaconfig.Alignment_ != 0)
        {
            oaconfig.InterAlignSize_ = static_cast<unsigned>(oaconfig.Alignment_ - (BlockSize % oaconfig.Alignment_));
        }
    }

    oastats.PageSize_ = sizeof(void *) + oaconfig.LeftAlignSize_ + (oaconfig.ObjectsPerPage_ * BlockSize);
    if (oaconfig.ObjectsPerPage_ > 1)
    {
        oastats.PageSize_ += (oaconfig.ObjectsPerPage_ - 1) * oaconfig.InterAlignSize_;
    }

    if (oaconfig.UseCPPMemManager_)
    {
        return;
    }

    AllocateNewPage();
}

/*!
  \brief Destroys the ObjectAllocator (never throws).
*/
ObjectAllocator::~ObjectAllocator()
{
    if (oaconfig.UseCPPMemManager_)
    {
        return;
    }

    GenericObject *current_page = PageList_;

    while (current_page != nullptr)
    {
        GenericObject *next_page = current_page->Next;

        if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            char *block_ptr = reinterpret_cast<char *>(current_page) + sizeof(void *) + oaconfig.LeftAlignSize_;
            size_t BlockSize = oaconfig.HBlockInfo_.size_ + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

            for (unsigned i = 0; i < oaconfig.ObjectsPerPage_; ++i)
            {
                char *data_ptr = block_ptr + oaconfig.HBlockInfo_.size_ + oaconfig.PadBytes_;
                MemBlockInfo **info_ptr = reinterpret_cast<MemBlockInfo **>(data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_);

                if (*info_ptr)
                {
                    if ((*info_ptr)->label)
                    {
                        delete[] (*info_ptr)->label;
                    }
                    delete *info_ptr;
                }
                block_ptr += BlockSize + oaconfig.InterAlignSize_;
            }
        }

        char *page_to_delete = reinterpret_cast<char *>(current_page);
        delete[] page_to_delete;
        current_page = next_page;
    }
}

/*!
  \brief Allocates a new object.
  \param label Optional label for external header.
  \return Pointer to the allocated data.
  \exception OAException If memory or page limit is reached.
*/
void *ObjectAllocator::Allocate(const char *label)
{
    if (oaconfig.UseCPPMemManager_)
    {
        try
        {
            char *ptr = new char[oastats.ObjectSize_];
            oastats.Allocations_++;
            oastats.ObjectsInUse_++;
            if (oastats.ObjectsInUse_ > oastats.MostObjects_)
                oastats.MostObjects_ = oastats.ObjectsInUse_;
            return ptr;
        }
        catch (const std::bad_alloc &)
        {
            throw OAException(OAException::E_NO_MEMORY, "C++ new failed.");
        }
    }

    if (FreeList_ == nullptr)
    {
        if (oaconfig.MaxPages_ != 0 && oastats.PagesInUse_ >= oaconfig.MaxPages_)
        {
            throw OAException(OAException::E_NO_MEMORY, "ObjectAllocator: Maximum pages reached.");
        }

        AllocateNewPage();
    }

    GenericObject *allocated_block = FreeList_;
    FreeList_ = FreeList_->Next;

    oastats.FreeObjects_--;
    oastats.ObjectsInUse_++;
    oastats.Allocations_++;
    if (oastats.ObjectsInUse_ > oastats.MostObjects_)
    {
        oastats.MostObjects_ = oastats.ObjectsInUse_;
    }

    char *data_ptr = reinterpret_cast<char *>(allocated_block);

    if (oaconfig.HBlockInfo_.type_ != OAConfig::hbNone)
    {
        char *header_ptr = data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_;

        if (oaconfig.HBlockInfo_.type_ == OAConfig::hbBasic)
        {
            unsigned *alloc_num = reinterpret_cast<unsigned *>(header_ptr);
            *alloc_num = oastats.Allocations_;
            char *flag = header_ptr + sizeof(unsigned);
            *flag = 1;
        }
        else if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExtended)
        {
            size_t add_size = oaconfig.HBlockInfo_.additional_;

            unsigned short *use_counter = reinterpret_cast<unsigned short *>(header_ptr + add_size);
            (*use_counter)++;

            unsigned *alloc_num = reinterpret_cast<unsigned *>(header_ptr + add_size + sizeof(unsigned short));
            *alloc_num = oastats.Allocations_;

            char *flag = header_ptr + add_size + sizeof(unsigned short) + sizeof(unsigned);
            *flag = 1;
        }
        else if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            MemBlockInfo *info = *reinterpret_cast<MemBlockInfo **>(header_ptr);
            info->in_use = true;
            info->alloc_num = oastats.Allocations_;
            if (label)
            {
                info->label = new char[std::strlen(label) + 1];
                std::strcpy(info->label, label);
            }
        }
    }

    if (oaconfig.DebugOn_)
    {
        std::memset(data_ptr, ALLOCATED_PATTERN, oastats.ObjectSize_);
    }

    return data_ptr;
}

/*!
  \brief Frees an object.
  \param Object Pointer to the object to free.
  \exception OAException On corruption, bad boundary, or double free.
*/
void ObjectAllocator::Free(void *Object)
{
    if (oaconfig.UseCPPMemManager_)
    {
        char *ptr = static_cast<char *>(Object);
        delete[] ptr;

        oastats.ObjectsInUse_--;
        oastats.Deallocations_++;
        return;
    }

    char *data_ptr = static_cast<char *>(Object);

    if (oaconfig.DebugOn_)
    {
        bool valid_boundary = false;
        GenericObject *current_page = PageList_;
        size_t BlockSize = oaconfig.HBlockInfo_.size_ + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

        while (current_page)
        {
            char *page_start = reinterpret_cast<char *>(current_page);
            char *page_end = page_start + oastats.PageSize_;
            if (data_ptr > page_start && data_ptr < page_end)
            {
                size_t offset = static_cast<size_t>(data_ptr - (page_start + sizeof(void *) + oaconfig.LeftAlignSize_ + oaconfig.HBlockInfo_.size_ + oaconfig.PadBytes_));
                if (offset % (BlockSize + oaconfig.InterAlignSize_) == 0)
                {
                    valid_boundary = true;
                    break;
                }
            }

            current_page = current_page->Next;
        }
        if (!valid_boundary)
        {
            throw OAException(OAException::E_BAD_BOUNDARY, "Invalid boundary.");
        }

        GenericObject *free_ptr = FreeList_;
        while (free_ptr)
        {
            if (reinterpret_cast<char *>(free_ptr) == data_ptr)
            {
                throw OAException(OAException::E_MULTIPLE_FREE, "Multiple free detected.");
            }
            free_ptr = free_ptr->Next;
        }

        if (oaconfig.PadBytes_ > 0)
        {
            char *left_pad = data_ptr - oaconfig.PadBytes_;
            char *right_pad = data_ptr + oastats.ObjectSize_;
            for (unsigned i = 0; i < oaconfig.PadBytes_; i++)
            {
                if (static_cast<unsigned char>(left_pad[i]) != PAD_PATTERN || static_cast<unsigned char>(right_pad[i]) != PAD_PATTERN)
                {
                    throw OAException(OAException::E_CORRUPTED_BLOCK, "Pad bytes corrupted.");
                }
            }
        }
    }

    if (oaconfig.HBlockInfo_.type_ != OAConfig::hbNone)
    {
        char *header_ptr = data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_;
        if (oaconfig.HBlockInfo_.type_ == OAConfig::hbBasic)
        {
            char *flag = header_ptr + sizeof(unsigned);
            *flag = 0;
            unsigned *alloc_num = reinterpret_cast<unsigned *>(header_ptr);
            *alloc_num = 0;
        }
        else if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExtended)
        {
            size_t add_size = oaconfig.HBlockInfo_.additional_;

            unsigned *alloc_num = reinterpret_cast<unsigned *>(header_ptr + add_size + sizeof(unsigned short));
            *alloc_num = 0;

            char *flag = header_ptr + add_size + sizeof(unsigned short) + sizeof(unsigned);
            *flag = 0;
        }
        else if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
        {
            MemBlockInfo *info = *reinterpret_cast<MemBlockInfo **>(header_ptr);
            info->in_use = false;
            info->alloc_num = 0;
            if (info->label)
            {
                delete[] info->label;
                info->label = nullptr;
            }
        }
    }

    if (oaconfig.DebugOn_)
    {
        std::memset(data_ptr, FREED_PATTERN, oastats.ObjectSize_);
    }

    GenericObject *pObj = static_cast<GenericObject *>(Object);
    pObj->Next = FreeList_;
    FreeList_ = pObj;

    oastats.ObjectsInUse_--;
    oastats.FreeObjects_++;
    oastats.Deallocations_++;
}

/*!
  \brief Frees empty pages.
  \return Number of freed pages.
*/
unsigned ObjectAllocator::FreeEmptyPages()
{
    if (oaconfig.UseCPPMemManager_)
        return 0;

    unsigned freed_pages_count = 0;
    GenericObject *current_page = PageList_;
    GenericObject *prev_page = nullptr;

    while (current_page != nullptr)
    {
        char *page_start = reinterpret_cast<char *>(current_page);
        char *page_end = page_start + oastats.PageSize_;

        unsigned free_blocks_in_page = 0;
        GenericObject *free_ptr = FreeList_;
        while (free_ptr != nullptr)
        {
            char *ptr = reinterpret_cast<char *>(free_ptr);
            if (ptr > page_start && ptr < page_end)
            {
                free_blocks_in_page++;
            }
            free_ptr = free_ptr->Next;
        }

        if (free_blocks_in_page == oaconfig.ObjectsPerPage_)
        {
            GenericObject *curr_free = FreeList_;
            GenericObject *prev_free = nullptr;

            while (curr_free != nullptr)
            {
                char *ptr = reinterpret_cast<char *>(curr_free);
                if (ptr > page_start && ptr < page_end)
                {
                    if (prev_free == nullptr)
                    {
                        FreeList_ = curr_free->Next;
                    }
                    else
                    {
                        prev_free->Next = curr_free->Next;
                    }
                    curr_free = curr_free->Next;

                    oastats.FreeObjects_--;
                }
                else
                {
                    prev_free = curr_free;
                    curr_free = curr_free->Next;
                }
            }

            if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
            {
                char *block_ptr = page_start + sizeof(void *) + oaconfig.LeftAlignSize_;
                size_t BlockSize = oaconfig.HBlockInfo_.size_ + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

                for (unsigned i = 0; i < oaconfig.ObjectsPerPage_; ++i)
                {
                    char *data_ptr = block_ptr + oaconfig.HBlockInfo_.size_ + oaconfig.PadBytes_;
                    MemBlockInfo **info_ptr = reinterpret_cast<MemBlockInfo **>(data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_);

                    if (*info_ptr)
                    {
                        if ((*info_ptr)->label)
                        {
                            delete[] (*info_ptr)->label;
                        }
                        delete *info_ptr;
                    }
                    block_ptr += BlockSize + oaconfig.InterAlignSize_;
                }
            }

            GenericObject *page_to_delete = current_page;
            if (prev_page == nullptr)
            {
                PageList_ = current_page->Next;
            }
            else
            {
                prev_page->Next = current_page->Next;
            }
            current_page = current_page->Next;

            delete[] reinterpret_cast<char *>(page_to_delete);
            oastats.PagesInUse_--;
            freed_pages_count++;
        }
        else
        {
            prev_page = current_page;
            current_page = current_page->Next;
        }
    }

    return freed_pages_count;
}

/*!
  \brief Dumps memory currently in use.
  \param fn Callback function.
  \return Number of blocks in use.
*/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    unsigned count = 0;
    GenericObject *page = PageList_;
    size_t BlockSize = oaconfig.HBlockInfo_.size_ + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

    while (page)
    {
        char *block_ptr = reinterpret_cast<char *>(page) + sizeof(void *) + oaconfig.LeftAlignSize_;
        for (unsigned i = 0; i < oaconfig.ObjectsPerPage_; i++)
        {
            char *data_ptr = block_ptr + oaconfig.HBlockInfo_.size_ + oaconfig.PadBytes_;
            bool in_use = false;

            if (oaconfig.HBlockInfo_.type_ == OAConfig::hbBasic || oaconfig.HBlockInfo_.type_ == OAConfig::hbExtended)
            {
                in_use = *(data_ptr - oaconfig.PadBytes_ - 1) == 1;
            }
            else if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
            {
                MemBlockInfo *info = *reinterpret_cast<MemBlockInfo **>(data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_);
                in_use = info->in_use;
            }
            else
            {
                in_use = (static_cast<unsigned char>(*data_ptr) == ALLOCATED_PATTERN);
            }

            if (in_use)
            {
                fn(data_ptr, oastats.ObjectSize_);
                count++;
            }
            block_ptr += BlockSize + oaconfig.InterAlignSize_;
        }

        page = page->Next;
    }

    return count;
}

/*!
  \brief Validates pad bytes for corruption.
  \param fn Callback function.
  \return Number of corrupted blocks.
*/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
    if (oaconfig.PadBytes_ == 0 || !oaconfig.DebugOn_)
        return 0;

    unsigned count = 0;
    GenericObject *page = PageList_;
    size_t BlockSize = oaconfig.HBlockInfo_.size_ + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;

    while (page)
    {
        char *block_ptr = reinterpret_cast<char *>(page) + sizeof(void *) + oaconfig.LeftAlignSize_;
        for (unsigned i = 0; i < oaconfig.ObjectsPerPage_; i++)
        {
            char *data_ptr = block_ptr + oaconfig.HBlockInfo_.size_ + oaconfig.PadBytes_;
            char *left_pad = data_ptr - oaconfig.PadBytes_;
            char *right_pad = data_ptr + oastats.ObjectSize_;
            bool corrupted = false;

            for (unsigned p = 0; p < oaconfig.PadBytes_; p++)
            {
                if (static_cast<unsigned char>(left_pad[p]) != PAD_PATTERN ||
                    static_cast<unsigned char>(right_pad[p]) != PAD_PATTERN)
                {
                    corrupted = true;
                    break;
                }
            }
            if (corrupted)
            {
                fn(data_ptr, oastats.ObjectSize_);
                count++;
            }
            block_ptr += BlockSize + oaconfig.InterAlignSize_;
        }
        page = page->Next;
    }
    return count;
}

/*!
  \brief Checks if extra credit features are implemented.
  \return true (implemented).
*/
bool ObjectAllocator::ImplementedExtraCredit()
{
    return true;
}

/*!
  \brief Sets debugging state.
  \param State True to enable, false to disable.
*/
void ObjectAllocator::SetDebugState(bool State)
{
    oaconfig.DebugOn_ = State;
}

/*!
  \brief Returns free list.
  \return Free list pointer.
*/
const void *ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

/*!
  \brief Returns page list.
  \return Page list pointer.
*/
const void *ObjectAllocator::GetPageList() const
{
    return PageList_;
}

/*!
  \brief Returns configuration.
  \return Config struct.
*/
OAConfig ObjectAllocator::GetConfig() const
{
    return oaconfig;
}

/*!
  \brief Returns statistics.
  \return Stats struct.
*/
OAStats ObjectAllocator::GetStats() const
{
    return oastats;
}

/*!
  \brief Allocates a new page and divides it into blocks.
  \exception OAException On memory allocation failure.
*/
void ObjectAllocator::AllocateNewPage()
{
    try
    {
        char *new_page = new char[oastats.PageSize_];

        if (oaconfig.DebugOn_)
        {
            std::memset(new_page, UNALLOCATED_PATTERN, oastats.PageSize_);
            if (oaconfig.LeftAlignSize_ > 0)
            {
                std::memset(new_page + sizeof(void *), ALIGN_PATTERN, oaconfig.LeftAlignSize_);
            }
        }

        GenericObject *pPage = reinterpret_cast<GenericObject *>(new_page);
        pPage->Next = PageList_;
        PageList_ = pPage;
        oastats.PagesInUse_++;

        size_t HBlockSize = oaconfig.HBlockInfo_.size_;
        size_t BlockSize = HBlockSize + (oaconfig.PadBytes_ * 2) + oastats.ObjectSize_;
        char *block_ptr = new_page + sizeof(void *) + oaconfig.LeftAlignSize_;

        for (unsigned i = 0; i < oaconfig.ObjectsPerPage_; ++i)
        {
            char *data_ptr = block_ptr + HBlockSize + oaconfig.PadBytes_;

            if (oaconfig.DebugOn_)
            {
                std::memset(data_ptr - oaconfig.PadBytes_, PAD_PATTERN, oaconfig.PadBytes_);
                std::memset(data_ptr + oastats.ObjectSize_, PAD_PATTERN, oaconfig.PadBytes_);

                if (i > 0 && oaconfig.InterAlignSize_ > 0)
                {
                    std::memset(block_ptr - oaconfig.InterAlignSize_, ALIGN_PATTERN, oaconfig.InterAlignSize_);
                }
            }

            if (oaconfig.HBlockInfo_.type_ == OAConfig::hbBasic || oaconfig.HBlockInfo_.type_ == OAConfig::hbExtended)
            {
                std::memset(data_ptr - oaconfig.PadBytes_ - HBlockSize, 0, HBlockSize);
            }

            if (oaconfig.HBlockInfo_.type_ == OAConfig::hbExternal)
            {
                MemBlockInfo **info_ptr = reinterpret_cast<MemBlockInfo **>(data_ptr - oaconfig.PadBytes_ - oaconfig.HBlockInfo_.size_);
                *info_ptr = new MemBlockInfo{false, nullptr, 0};
            }

            GenericObject *pObj = reinterpret_cast<GenericObject *>(data_ptr);
            pObj->Next = FreeList_;
            FreeList_ = pObj;
            oastats.FreeObjects_++;

            block_ptr += BlockSize + oaconfig.InterAlignSize_;
        }
    }
    catch (const std::bad_alloc &)
    {
        throw OAException(OAException::E_NO_MEMORY, "ObjectAllocator: Out of memory.");
    }
}