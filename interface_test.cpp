#include "xxfl_set_test.h"

static const uint32_t def_insert_count = 10000;

template<typename _int_set>
void int_set_interface_test()
{
    typedef typename _int_set::iterator               _iterator;
    typedef typename _int_set::reverse_iterator       _reverse_iterator;
    typedef typename _int_set::const_iterator         _const_iterator;
    typedef typename _int_set::const_reverse_iterator _const_reverse_iterator;

    bool success = true;

    _int_set xx, yy;
    container_insert_random_1(xx, def_insert_count);
    container_insert_sequential(yy, def_insert_count);

    {
        def_int_compare compare;
        def_allocator allocator;

        _int_set aa(compare);
        _int_set bb(compare, allocator);
        _int_set cc(allocator);

        std_int_vector vec;
        int_vector_append_sequential(vec, def_insert_count);

        _int_set dd(vec.begin(), vec.end());
        _int_set ee(vec.begin(), vec.end(), compare);
        _int_set ff(vec.rbegin(), vec.rend(), compare, allocator);
        _int_set gg(vec.rbegin(), vec.rend(), allocator);

        success &= (dd.size() == def_insert_count &&
                    ee.size() == def_insert_count &&
                    ff.size() == def_insert_count &&
                    gg.size() == def_insert_count);

        _int_set hh(xx);
        _int_set ii(xx, allocator);

        success &= (hh.size() == xx.size() &&
                    ii.size() == xx.size());

        _int_set jj(std::move(ii));
        _int_set kk(std::move(jj), allocator);

        success &= (ii.size() == 0 &&
                    jj.size() == 0 &&
                    kk.size() == xx.size());

        _int_set ll({ 1, 2, 3, 4, 5 });
        _int_set mm({ 1, 2, 3, 4, 5 }, allocator);

        success &= (ll.size() == 5 &&
                    mm.size() == 5);
    }

    {
        _int_set aa, bb, cc;
        aa = xx;
        bb = _int_set(xx);
        cc = { 1, 2, 3, 4, 5 };

        success &= (aa.size() == xx.size() &&
                    bb.size() == xx.size() &&
                    cc.size() == 5);

        typename _int_set::key_compare kcomp(aa.key_comp());
        typename _int_set::value_compare vcomp(aa.value_comp());
        typename _int_set::allocator_type alloc(aa.get_allocator());

        success &= (kcomp(11111, 22222) &&
                    vcomp(11111, 22222));
    }

    {
        _iterator it_begin(yy.begin());
        _iterator it_end(yy.end());
        _const_iterator it_cbegin(yy.cbegin());
        _const_iterator it_cend(yy.cend());

        _reverse_iterator it_rbegin(yy.rbegin());
        _reverse_iterator it_rend(yy.rend());
        _const_reverse_iterator it_crbegin(yy.crbegin());
        _const_reverse_iterator it_crend(yy.crend());

        success &= (it_begin != it_end && !(it_begin == it_end) &&
                    it_cbegin != it_cend && !(it_cbegin == it_cend) &&
                    it_rbegin != it_rend && !(it_rbegin == it_rend) &&
                    it_crbegin != it_crend && !(it_crbegin == it_crend));

        success &= (it_begin == it_cbegin && it_begin != it_cend &&
                    it_end == it_cend && it_end != it_cbegin &&
                    it_rbegin == it_crbegin && it_rbegin != it_crend &&
                    it_rend == it_crend && it_rend != it_crbegin);


        _iterator it(it_begin);
        for (uint32_t i = 0; it != it_end; ++i, ++it)
        {
            success &= *it == i;
        }
        success &= *(--it) == def_insert_count - 1;
        success &= *(it--) == def_insert_count - 1;
        success &= *(it++) == def_insert_count - 2;
        success &= *it == def_insert_count - 1;

        _const_iterator cit(it_cbegin);
        for (uint32_t i = 0; cit != it_cend; ++i, ++cit)
        {
            success &= *cit == i;
        }
        success &= *(--cit) == def_insert_count - 1;
        success &= *(cit--) == def_insert_count - 1;
        success &= *(cit++) == def_insert_count - 2;
        success &= *cit == def_insert_count - 1;

        _reverse_iterator rit(it_rbegin);
        for (uint32_t i = 0; rit != it_rend; ++i, ++rit)
        {
            success &= *rit == def_insert_count - 1 - i;
        }
        success &= *(--rit) == 0;
        success &= *(rit--) == 0;
        success &= *(rit++) == 1;
        success &= *rit == 0;

        _const_reverse_iterator crit(it_crbegin);
        for (uint32_t i = 0; crit != it_crend; ++i, ++crit)
        {
            success &= *crit == def_insert_count - 1 - i;
        }
        success &= *(--crit) == 0;
        success &= *(crit--) == 0;
        success &= *(crit++) == 1;
        success &= *crit == 0;
    }

    {
        _int_set aa(xx);
        _int_set bb({ 1, 2, 3, 4, 5 });
        _int_set cc({ 1, 2, 4, 5, 6 });

        success &= (!aa.empty() && aa.size() < aa.max_size());

        aa.swap(bb);
        success &= (aa.size() == 5 && bb.size() == xx.size());

        std::swap(aa, bb);
        success &= (aa.size() == xx.size() && bb.size() == 5);

        success &= (aa == xx && aa <= xx && aa >= xx);
        success &= (bb != cc && !(bb == cc) &&
                    bb < cc && !(bb > cc) &&
                    bb <= cc && !(bb >= cc));
    }

    {
        _int_set aa({ 1, 3, 5, 7, 9 });

        auto rp = aa.emplace(2);
        success &= (aa.size() == 6 && rp.second && *rp.first == 2);

        rp = aa.emplace(3);
        success &= (aa.size() == 6 && !rp.second && *rp.first == 3);

        _iterator it = aa.emplace_hint(aa.find(5), 4);
        success &= (aa.size() == 7 && *it == 4);

        it = aa.emplace_hint(aa.cbegin(), 5);
        success &= (aa.size() == 7 && *it == 5);

        test_int val = 6;
        rp = aa.insert(val);
        success &= (aa.size() == 8 && rp.second && *rp.first == 6);

        rp = aa.insert(7);
        success &= (aa.size() == 8 && !rp.second && *rp.first == 7);

        val = 8;
        it = aa.insert(aa.find(9), val);
        success &= (aa.size() == 9 && *it == 8);

        it = aa.insert(aa.cbegin(), 9);
        success &= (aa.size() == 9 && *it == 9);

        std_int_vector vec({ 8, 9, 10, 11, 12 });
        aa.insert(vec.begin(), vec.end());
        success &= aa.size() == 12;

        aa.insert({ 11, 12, 13, 14, 15 });
        success &= aa.size() == 15;
    }

    {
        _int_set aa(yy);

        _iterator it = aa.erase(aa.cbegin());
        success &= (aa.size() == def_insert_count - 1 && *it == 1);

        it = aa.erase(aa.cend());
        success &= (aa.size() == def_insert_count - 1 && it == aa.end());

        size_t ret = aa.erase(100);
        success &= (aa.size() == def_insert_count - 2 && ret == 1);

        ret = aa.erase(def_insert_count);
        success &= (aa.size() == def_insert_count - 2 && ret == 0);

        it = aa.erase(aa.find(200), aa.find(300));
        success &= (aa.size() == def_insert_count - 102 && *it == 300);
        success &= *(--it) == 199;

        it = aa.erase(aa.find(def_insert_count - 100), aa.cend());
        success &= (aa.size() == def_insert_count - 202 && it == aa.end());
        success &= *(--it) == def_insert_count - 101;

        aa.clear();
        success &= aa.size() == 0;
    }

    {
        _int_set aa(yy);

        success &= (aa.count(100) == 1 &&
                    aa.count(def_insert_count) == 0);

        _iterator it = aa.find(200);
        success &= *it == 200;

        it = aa.find(def_insert_count + 100);
        success &= it == aa.end();

        aa.erase(0);
        it = aa.lower_bound(0);
        success &= *it == 1;

        it = aa.lower_bound(def_insert_count - 1);
        success &= *it == def_insert_count - 1;

        it = aa.upper_bound(300);
        success &= *it == 301;

        it = aa.upper_bound(def_insert_count - 1);
        success &= it == aa.end();

        auto rp = aa.equal_range(400);
        success &= (*rp.first == 400 &&
                    *rp.second == 401);

        rp = aa.equal_range(def_insert_count - 1);
        success &= (*rp.first == def_insert_count - 1 &&
                    rp.second == aa.end());
    }

    std::printf("%s\n", success? "passed" : "error");
}

template<typename _string_set>
void string_set_interface_test()
{
    typedef typename _string_set::iterator               _iterator;
    typedef typename _string_set::reverse_iterator       _reverse_iterator;
    typedef typename _string_set::const_iterator         _const_iterator;
    typedef typename _string_set::const_reverse_iterator _const_reverse_iterator;

    bool success = true;

    _string_set xx, yy;
    container_insert_random_1(xx, def_insert_count);
    container_insert_sequential(yy, def_insert_count);

    {
        def_string_compare compare;
        def_allocator allocator;

        _string_set aa(compare);
        _string_set bb(compare, allocator);
        _string_set cc(allocator);

        std_string_vector vec;
        string_vector_append_sequential(vec, def_insert_count);

        _string_set dd(vec.begin(), vec.end());
        _string_set ee(vec.begin(), vec.end(), compare);
        _string_set ff(vec.rbegin(), vec.rend(), compare, allocator);
        _string_set gg(vec.rbegin(), vec.rend(), allocator);

        success &= (dd.size() == def_insert_count &&
                    ee.size() == def_insert_count &&
                    ff.size() == def_insert_count &&
                    gg.size() == def_insert_count);

        _string_set hh(xx);
        _string_set ii(xx, allocator);

        success &= (hh.size() == xx.size() &&
                    ii.size() == xx.size());

        _string_set jj(std::move(ii));
        _string_set kk(std::move(jj), allocator);

        success &= (ii.size() == 0 &&
                    jj.size() == 0 &&
                    kk.size() == xx.size());

        _string_set ll({ "1", "2", "3", "4", "5" });
        _string_set mm({ "1", "2", "3", "4", "5" }, allocator);

        success &= (ll.size() == 5 &&
                    mm.size() == 5);
    }

    {
        _string_set aa, bb, cc;
        aa = xx;
        bb = _string_set(xx);
        cc = { "1", "2", "3", "4", "5" };

        success &= (aa.size() == xx.size() &&
                    bb.size() == xx.size() &&
                    cc.size() == 5);

        typename _string_set::key_compare kcomp(aa.key_comp());
        typename _string_set::value_compare vcomp(aa.value_comp());
        typename _string_set::allocator_type alloc(aa.get_allocator());

        success &= (kcomp(std::string("11111"), std::string("22222")) &&
                    vcomp(std::string("11111"), std::string("22222")));
    }

    {
        _iterator it_begin(yy.begin());
        _iterator it_end(yy.end());
        _const_iterator it_cbegin(yy.cbegin());
        _const_iterator it_cend(yy.cend());

        _reverse_iterator it_rbegin(yy.rbegin());
        _reverse_iterator it_rend(yy.rend());
        _const_reverse_iterator it_crbegin(yy.crbegin());
        _const_reverse_iterator it_crend(yy.crend());

        success &= (it_begin != it_end && !(it_begin == it_end) &&
                    it_cbegin != it_cend && !(it_cbegin == it_cend) &&
                    it_rbegin != it_rend && !(it_rbegin == it_rend) &&
                    it_crbegin != it_crend && !(it_crbegin == it_crend));

        success &= (it_begin == it_cbegin && it_begin != it_cend &&
                    it_end == it_cend && it_end != it_cbegin &&
                    it_rbegin == it_crbegin && it_rbegin != it_crend &&
                    it_rend == it_crend && it_rend != it_crbegin);


        _iterator it(it_begin);
        for (uint32_t i = 0; it != it_end; ++i, ++it)
        {
            success &= *it == number_to_string(i);
        }
        success &= *(--it) == number_to_string(def_insert_count - 1);
        success &= *(it--) == number_to_string(def_insert_count - 1);
        success &= *(it++) == number_to_string(def_insert_count - 2);
        success &= *it == number_to_string(def_insert_count - 1);

        _const_iterator cit(it_cbegin);
        for (uint32_t i = 0; cit != it_cend; ++i, ++cit)
        {
            success &= *cit == number_to_string(i);
        }
        success &= *(--cit) == number_to_string(def_insert_count - 1);
        success &= *(cit--) == number_to_string(def_insert_count - 1);
        success &= *(cit++) == number_to_string(def_insert_count - 2);
        success &= *cit == number_to_string(def_insert_count - 1);

        _reverse_iterator rit(it_rbegin);
        for (uint32_t i = 0; rit != it_rend; ++i, ++rit)
        {
            success &= *rit == number_to_string(def_insert_count - 1 - i);
        }
        success &= *(--rit) == number_to_string(0);
        success &= *(rit--) == number_to_string(0);
        success &= *(rit++) == number_to_string(1);
        success &= *rit == number_to_string(0);

        _const_reverse_iterator crit(it_crbegin);
        for (uint32_t i = 0; crit != it_crend; ++i, ++crit)
        {
            success &= *crit == number_to_string(def_insert_count - 1 - i);
        }
        success &= *(--crit) == number_to_string(0);
        success &= *(crit--) == number_to_string(0);
        success &= *(crit++) == number_to_string(1);
        success &= *crit == number_to_string(0);
    }

    {
        _string_set aa(xx);
        _string_set bb({ "1", "2", "3", "4", "5" });
        _string_set cc({ "1", "2", "4", "5", "6" });

        success &= (!aa.empty() && aa.size() < aa.max_size());

        aa.swap(bb);
        success &= (aa.size() == 5 && bb.size() == xx.size());

        std::swap(aa, bb);
        success &= (aa.size() == xx.size() && bb.size() == 5);

        success &= (aa == xx && aa <= xx && aa >= xx);
        success &= (bb != cc && !(bb == cc) &&
                    bb < cc && !(bb > cc) &&
                    bb <= cc && !(bb >= cc));
    }

    {
        _string_set aa({ "1", "3", "5", "7", "9" });

        auto rp = aa.emplace("2");
        success &= (aa.size() == 6 && rp.second && *rp.first == "2");

        rp = aa.emplace("3");
        success &= (aa.size() == 6 && !rp.second && *rp.first == "3");

        _iterator it = aa.emplace_hint(aa.find(std::string("5")), "4");
        success &= (aa.size() == 7 && *it == "4");

        it = aa.emplace_hint(aa.cbegin(), "5");
        success &= (aa.size() == 7 && *it == "5");

        std::string val("6");
        rp = aa.insert(val);
        success &= (aa.size() == 8 && rp.second && *rp.first == "6");

        rp = aa.insert(std::string("7"));
        success &= (aa.size() == 8 && !rp.second && *rp.first == "7");

        val = "8";
        it = aa.insert(aa.find(std::string("9")), val);
        success &= (aa.size() == 9 && *it == "8");

        it = aa.insert(aa.cbegin(), std::string("9"));
        success &= (aa.size() == 9 && *it == "9");

        std_string_vector vec({ "8", "9", "10", "11", "12" });
        aa.insert(vec.begin(), vec.end());
        success &= aa.size() == 12;

        aa.insert({ "11", "12", "13", "14", "15" });
        success &= aa.size() == 15;
    }

    {
        _string_set aa(yy);

        _iterator it = aa.erase(aa.cbegin());
        success &= (aa.size() == def_insert_count - 1 && *it == number_to_string(1));

        it = aa.erase(aa.cend());
        success &= (aa.size() == def_insert_count - 1 && it == aa.end());

        size_t ret = aa.erase(number_to_string(100));
        success &= (aa.size() == def_insert_count - 2 && ret == 1);

        ret = aa.erase(number_to_string(def_insert_count));
        success &= (aa.size() == def_insert_count - 2 && ret == 0);

        it = aa.erase(aa.find(number_to_string(200)), aa.find(number_to_string(300)));
        success &= (aa.size() == def_insert_count - 102 && *it == number_to_string(300));
        success &= *(--it) == number_to_string(199);

        it = aa.erase(aa.find(number_to_string(def_insert_count - 100)), aa.cend());
        success &= (aa.size() == def_insert_count - 202 && it == aa.end());
        success &= *(--it) == number_to_string(def_insert_count - 101);

        aa.clear();
        success &= aa.size() == 0;
    }

    {
        _string_set aa(yy);

        success &= (aa.count(number_to_string(100)) == 1 &&
                    aa.count(number_to_string(def_insert_count)) == 0);

        _iterator it = aa.find(number_to_string(200));
        success &= *it == number_to_string(200);

        it = aa.find(number_to_string(def_insert_count + 100));
        success &= it == aa.end();

        aa.erase(number_to_string(0));
        it = aa.lower_bound(number_to_string(0));
        success &= *it == number_to_string(1);

        it = aa.lower_bound(number_to_string(def_insert_count - 1));
        success &= *it == number_to_string(def_insert_count - 1);

        it = aa.upper_bound(number_to_string(300));
        success &= *it == number_to_string(301);

        it = aa.upper_bound(number_to_string(def_insert_count - 1));
        success &= it == aa.end();

        auto rp = aa.equal_range(number_to_string(400));
        success &= (*rp.first == number_to_string(400) &&
                    *rp.second == number_to_string(401));

        rp = aa.equal_range(number_to_string(def_insert_count - 1));
        success &= (*rp.first == number_to_string(def_insert_count - 1) &&
                    rp.second == aa.end());
    }

    std::printf("%s\n", success? "passed" : "error");
}

template<typename _int_map>
void int_map_interface_test()
{
    typedef typename _int_map::iterator               _iterator;
    typedef typename _int_map::reverse_iterator       _reverse_iterator;
    typedef typename _int_map::const_iterator         _const_iterator;
    typedef typename _int_map::const_reverse_iterator _const_reverse_iterator;

    bool success = true;

    _int_map xx, yy;
    container_insert_random_1(xx, def_insert_count);
    container_insert_sequential(yy, def_insert_count);

    {
        def_int_compare compare;
        def_allocator allocator;

        _int_map aa(compare);
        _int_map bb(compare, allocator);
        _int_map cc(allocator);

        std_int_pair_vector vec;
        int_pair_vector_append_sequential(vec, def_insert_count);

        _int_map dd(vec.begin(), vec.end());
        _int_map ee(vec.begin(), vec.end(), compare);
        _int_map ff(vec.rbegin(), vec.rend(), compare, allocator);
        _int_map gg(vec.rbegin(), vec.rend(), allocator);

        success &= (dd.size() == def_insert_count &&
                    ee.size() == def_insert_count &&
                    ff.size() == def_insert_count &&
                    gg.size() == def_insert_count);

        _int_map hh(xx);
        _int_map ii(xx, allocator);

        success &= (hh.size() == xx.size() &&
                    ii.size() == xx.size());

        _int_map jj(std::move(ii));
        _int_map kk(std::move(jj), allocator);

        success &= (ii.size() == 0 &&
                    jj.size() == 0 &&
                    kk.size() == xx.size());

        _int_map ll({ int_pair(1, 1), int_pair(2, 2), int_pair(3, 3), int_pair(4, 4), int_pair(5, 5) });
        _int_map mm({ int_pair(1, 1), int_pair(2, 2), int_pair(3, 3), int_pair(4, 4), int_pair(5, 5) }, allocator);

        success &= (ll.size() == 5 &&
                    mm.size() == 5);
    }

    {
        _int_map aa, bb, cc;
        aa = xx;
        bb = _int_map(xx);
        cc = { int_pair(1, 1), int_pair(2, 2), int_pair(3, 3), int_pair(4, 4), int_pair(5, 5) };

        success &= (aa.size() == xx.size() &&
                    bb.size() == xx.size() &&
                    cc.size() == 5);

        typename _int_map::key_compare kcomp(aa.key_comp());
        typename _int_map::value_compare vcomp(aa.value_comp());
        typename _int_map::allocator_type alloc(aa.get_allocator());

        success &= (kcomp(11111, 22222) &&
                    vcomp(int_pair(11111, 11111), int_pair(22222, 22222)));
    }

    {
        _iterator it_begin(yy.begin());
        _iterator it_end(yy.end());
        _const_iterator it_cbegin(yy.cbegin());
        _const_iterator it_cend(yy.cend());

        _reverse_iterator it_rbegin(yy.rbegin());
        _reverse_iterator it_rend(yy.rend());
        _const_reverse_iterator it_crbegin(yy.crbegin());
        _const_reverse_iterator it_crend(yy.crend());

        success &= (it_begin != it_end && !(it_begin == it_end) &&
                    it_cbegin != it_cend && !(it_cbegin == it_cend) &&
                    it_rbegin != it_rend && !(it_rbegin == it_rend) &&
                    it_crbegin != it_crend && !(it_crbegin == it_crend));

        success &= (it_begin == it_cbegin && it_begin != it_cend &&
                    it_end == it_cend && it_end != it_cbegin &&
                    it_rbegin == it_crbegin && it_rbegin != it_crend &&
                    it_rend == it_crend && it_rend != it_crbegin);


        _iterator it(it_begin);
        for (uint32_t i = 0; it != it_end; ++i, ++it)
        {
            success &= it->first == i;
        }
        success &= (--it)->first == def_insert_count - 1;
        success &= (it--)->first == def_insert_count - 1;
        success &= (it++)->first == def_insert_count - 2;
        success &= it->first == def_insert_count - 1;

        _const_iterator cit(it_cbegin);
        for (uint32_t i = 0; cit != it_cend; ++i, ++cit)
        {
            success &= cit->first == i;
        }
        success &= (--cit)->first == def_insert_count - 1;
        success &= (cit--)->first == def_insert_count - 1;
        success &= (cit++)->first == def_insert_count - 2;
        success &= cit->first == def_insert_count - 1;

        _reverse_iterator rit(it_rbegin);
        for (uint32_t i = 0; rit != it_rend; ++i, ++rit)
        {
            success &= rit->first == def_insert_count - 1 - i;
        }
        success &= (--rit)->first == 0;
        success &= (rit--)->first == 0;
        success &= (rit++)->first == 1;
        success &= rit->first == 0;

        _const_reverse_iterator crit(it_crbegin);
        for (uint32_t i = 0; crit != it_crend; ++i, ++crit)
        {
            success &= crit->first == def_insert_count - 1 - i;
        }
        success &= (--crit)->first == 0;
        success &= (crit--)->first == 0;
        success &= (crit++)->first == 1;
        success &= crit->first == 0;
    }

    {
        _int_map aa(xx);
        _int_map bb({ int_pair(1, 1), int_pair(2, 2), int_pair(3, 3), int_pair(4, 4), int_pair(5, 5) });
        _int_map cc({ int_pair(1, 1), int_pair(2, 2), int_pair(4, 4), int_pair(5, 5), int_pair(6, 6) });

        success &= (!aa.empty() && aa.size() < aa.max_size());

        aa.swap(bb);
        success &= (aa.size() == 5 && bb.size() == xx.size());

        std::swap(aa, bb);
        success &= (aa.size() == xx.size() && bb.size() == 5);

        success &= (aa == xx && aa <= xx && aa >= xx);
        success &= (bb != cc && !(bb == cc) &&
                    bb < cc && !(bb > cc) &&
                    bb <= cc && !(bb >= cc));
    }

    {
        _int_map aa({ int_pair(1, 1), int_pair(3, 3), int_pair(5, 5), int_pair(7, 7), int_pair(9, 9) });

        auto rp = aa.emplace(2, 2);
        success &= (aa.size() == 6 && rp.second && rp.first->first == 2);

        rp = aa.emplace(3, 3);
        success &= (aa.size() == 6 && !rp.second && rp.first->first == 3);

        _iterator it = aa.emplace_hint(aa.find(5), 4, 4);
        success &= (aa.size() == 7 && it->first == 4);

        it = aa.emplace_hint(aa.cbegin(), 5, 5);
        success &= (aa.size() == 7 && it->first == 5);

        int_pair val_1(6, 6);
        rp = aa.insert(val_1);
        success &= (aa.size() == 8 && rp.second && rp.first->first == 6);

        rp = aa.insert(int_pair(7, 7));
        success &= (aa.size() == 8 && !rp.second && rp.first->first == 7);

        int_pair val_2(8, 8);
        it = aa.insert(aa.find(9), val_2);
        success &= (aa.size() == 9 && it->first == 8);

        it = aa.insert(aa.cbegin(), int_pair(9, 9));
        success &= (aa.size() == 9 && it->first == 9);

        std_int_pair_vector vec({ int_pair(8, 8), int_pair(9, 9), int_pair(10, 10), int_pair(11, 11), int_pair(12, 12) });
        aa.insert(vec.begin(), vec.end());
        success &= aa.size() == 12;

        aa.insert({ int_pair(11, 11), int_pair(12, 12), int_pair(13, 13), int_pair(14, 14), int_pair(15, 15) });
        success &= aa.size() == 15;
    }

    {
        _int_map aa(yy);

        _iterator it = aa.erase(aa.cbegin());
        success &= (aa.size() == def_insert_count - 1 && it->first == 1);

        it = aa.erase(aa.cend());
        success &= (aa.size() == def_insert_count - 1 && it == aa.end());

        size_t ret = aa.erase(100);
        success &= (aa.size() == def_insert_count - 2 && ret == 1);

        ret = aa.erase(def_insert_count);
        success &= (aa.size() == def_insert_count - 2 && ret == 0);

        it = aa.erase(aa.find(200), aa.find(300));
        success &= (aa.size() == def_insert_count - 102 && it->first == 300);
        success &= (--it)->first == 199;

        it = aa.erase(aa.find(def_insert_count - 100), aa.cend());
        success &= (aa.size() == def_insert_count - 202 && it == aa.end());
        success &= (--it)->first == def_insert_count - 101;

        aa.clear();
        success &= aa.size() == 0;
    }

    {
        _int_map aa(yy);

        success &= (aa.count(100) == 1 &&
                    aa.count(def_insert_count) == 0);

        _iterator it = aa.find(200);
        success &= it->first == 200;

        it = aa.find(def_insert_count + 100);
        success &= it == aa.end();

        aa.erase(0);
        it = aa.lower_bound(0);
        success &= it->first == 1;

        it = aa.lower_bound(def_insert_count - 1);
        success &= it->first == def_insert_count - 1;

        it = aa.upper_bound(300);
        success &= it->first == 301;

        it = aa.upper_bound(def_insert_count - 1);
        success &= it == aa.end();

        auto rp = aa.equal_range(400);
        success &= (rp.first->first == 400 &&
                    rp.second->first == 401);

        rp = aa.equal_range(def_insert_count - 1);
        success &= (rp.first->first == def_insert_count - 1 &&
                    rp.second == aa.end());
    }

    {
        _int_map aa({ int_pair(1, 1), int_pair(2, 2), int_pair(3, 3), int_pair(4, 4), int_pair(5, 5) });

        test_int key = 2;
        success &= aa[key] == 2;

        aa[key] = 200;
        success &= (aa.size() == 5 && aa.find(2)->second == 200);

        key = 6;
        aa[key] = 600;
        success &= (aa.size() == 6 && aa.find(6)->second == 600);

        success &= aa[3] == 3;

        aa[3] = 300;
        success &= (aa.size() == 6 && aa.find(3)->second == 300);

        aa[7] = 700;
        success &= (aa.size() == 7 && aa.find(7)->second == 700);

        key = 4;
        success &= aa.at(4) == 4;

        aa[key] = 400;
        success &= (aa.size() == 7 && aa.find(4)->second == 400);

        bool is_throw = false;
        try
        {
            aa.at(8) = 800;
        }
        catch(...)
        {
            is_throw = true;
        }
        success &= is_throw;
    }

    std::printf("%s\n", success? "passed" : "error");
}

template<typename _string_map>
void string_map_interface_test()
{
    typedef typename _string_map::iterator               _iterator;
    typedef typename _string_map::reverse_iterator       _reverse_iterator;
    typedef typename _string_map::const_iterator         _const_iterator;
    typedef typename _string_map::const_reverse_iterator _const_reverse_iterator;

    bool success = true;

    _string_map xx, yy;
    container_insert_random_1(xx, def_insert_count);
    container_insert_sequential(yy, def_insert_count);

    {
        def_string_compare compare;
        def_allocator allocator;

        _string_map aa(compare);
        _string_map bb(compare, allocator);
        _string_map cc(allocator);

        std_string_pair_vector vec;
        string_pair_vector_append_sequential(vec, def_insert_count);

        _string_map dd(vec.begin(), vec.end());
        _string_map ee(vec.begin(), vec.end(), compare);
        _string_map ff(vec.rbegin(), vec.rend(), compare, allocator);
        _string_map gg(vec.rbegin(), vec.rend(), allocator);

        success &= (dd.size() == def_insert_count &&
                    ee.size() == def_insert_count &&
                    ff.size() == def_insert_count &&
                    gg.size() == def_insert_count);

        _string_map hh(xx);
        _string_map ii(xx, allocator);

        success &= (hh.size() == xx.size() &&
                    ii.size() == xx.size());

        _string_map jj(std::move(ii));
        _string_map kk(std::move(jj), allocator);

        success &= (ii.size() == 0 &&
                    jj.size() == 0 &&
                    kk.size() == xx.size());

        _string_map ll({ string_pair("1", "1"), string_pair("2", "2"), string_pair("3", "3"), string_pair("4", "4"), string_pair("5", "5") });
        _string_map mm({ string_pair("1", "1"), string_pair("2", "2"), string_pair("3", "3"), string_pair("4", "4"), string_pair("5", "5") }, allocator);

        success &= (ll.size() == 5 &&
                    mm.size() == 5);
    }

    {
        _string_map aa, bb, cc;
        aa = xx;
        bb = _string_map(xx);
        cc = { string_pair("1", "1"), string_pair("2", "2"), string_pair("3", "3"), string_pair("4", "4"), string_pair("5", "5") };

        success &= (aa.size() == xx.size() &&
                    bb.size() == xx.size() &&
                    cc.size() == 5);

        typename _string_map::key_compare kcomp(aa.key_comp());
        typename _string_map::value_compare vcomp(aa.value_comp());
        typename _string_map::allocator_type alloc(aa.get_allocator());

        success &= (kcomp(std::string("11111"), std::string("22222")) &&
                    vcomp(string_pair("11111", "11111"), string_pair("22222", "22222")));
    }

    {
        _iterator it_begin(yy.begin());
        _iterator it_end(yy.end());
        _const_iterator it_cbegin(yy.cbegin());
        _const_iterator it_cend(yy.cend());

        _reverse_iterator it_rbegin(yy.rbegin());
        _reverse_iterator it_rend(yy.rend());
        _const_reverse_iterator it_crbegin(yy.crbegin());
        _const_reverse_iterator it_crend(yy.crend());

        success &= (it_begin != it_end && !(it_begin == it_end) &&
                    it_cbegin != it_cend && !(it_cbegin == it_cend) &&
                    it_rbegin != it_rend && !(it_rbegin == it_rend) &&
                    it_crbegin != it_crend && !(it_crbegin == it_crend));

        success &= (it_begin == it_cbegin && it_begin != it_cend &&
                    it_end == it_cend && it_end != it_cbegin &&
                    it_rbegin == it_crbegin && it_rbegin != it_crend &&
                    it_rend == it_crend && it_rend != it_crbegin);


        _iterator it(it_begin);
        for (uint32_t i = 0; it != it_end; ++i, ++it)
        {
            success &= it->first == number_to_string(i);
        }
        success &= (--it)->first == number_to_string(def_insert_count - 1);
        success &= (it--)->first == number_to_string(def_insert_count - 1);
        success &= (it++)->first == number_to_string(def_insert_count - 2);
        success &= it->first == number_to_string(def_insert_count - 1);

        _const_iterator cit(it_cbegin);
        for (uint32_t i = 0; cit != it_cend; ++i, ++cit)
        {
            success &= cit->first == number_to_string(i);
        }
        success &= (--cit)->first == number_to_string(def_insert_count - 1);
        success &= (cit--)->first == number_to_string(def_insert_count - 1);
        success &= (cit++)->first == number_to_string(def_insert_count - 2);
        success &= cit->first == number_to_string(def_insert_count - 1);

        _reverse_iterator rit(it_rbegin);
        for (uint32_t i = 0; rit != it_rend; ++i, ++rit)
        {
            success &= rit->first == number_to_string(def_insert_count - 1 - i);
        }
        success &= (--rit)->first == number_to_string(0);
        success &= (rit--)->first == number_to_string(0);
        success &= (rit++)->first == number_to_string(1);
        success &= rit->first == number_to_string(0);

        _const_reverse_iterator crit(it_crbegin);
        for (uint32_t i = 0; crit != it_crend; ++i, ++crit)
        {
            success &= crit->first == number_to_string(def_insert_count - 1 - i);
        }
        success &= (--crit)->first == number_to_string(0);
        success &= (crit--)->first == number_to_string(0);
        success &= (crit++)->first == number_to_string(1);
        success &= crit->first == number_to_string(0);
    }

    {
        _string_map aa(xx);
        _string_map bb({ string_pair("1", "1"), string_pair("2", "2"), string_pair("3", "3"), string_pair("4", "4"), string_pair("5", "5") });
        _string_map cc({ string_pair("1", "1"), string_pair("2", "2"), string_pair("4", "4"), string_pair("5", "5"), string_pair("6", "6") });

        success &= (!aa.empty() && aa.size() < aa.max_size());

        aa.swap(bb);
        success &= (aa.size() == 5 && bb.size() == xx.size());

        std::swap(aa, bb);
        success &= (aa.size() == xx.size() && bb.size() == 5);

        success &= (aa == xx && aa <= xx && aa >= xx);
        success &= (bb != cc && !(bb == cc) &&
                    bb < cc && !(bb > cc) &&
                    bb <= cc && !(bb >= cc));
    }

    {
        _string_map aa({ string_pair("1", "1"), string_pair("3", "3"), string_pair("5", "5"), string_pair("7", "7"), string_pair("9", "9") });

        auto rp = aa.emplace("2", "2");
        success &= (aa.size() == 6 && rp.second && rp.first->first == "2");

        rp = aa.emplace("3", "3");
        success &= (aa.size() == 6 && !rp.second && rp.first->first == "3");

        _iterator it = aa.emplace_hint(aa.find("5"), "4", "4");
        success &= (aa.size() == 7 && it->first == "4");

        it = aa.emplace_hint(aa.cbegin(), "5", "5");
        success &= (aa.size() == 7 && it->first == "5");

        string_pair val_1("6", "6");
        rp = aa.insert(val_1);
        success &= (aa.size() == 8 && rp.second && rp.first->first == "6");

        rp = aa.insert(string_pair("7", "7"));
        success &= (aa.size() == 8 && !rp.second && rp.first->first == "7");

        string_pair val_2("8", "8");
        it = aa.insert(aa.find("9"), val_2);
        success &= (aa.size() == 9 && it->first == "8");

        it = aa.insert(aa.cbegin(), string_pair("9", "9"));
        success &= (aa.size() == 9 && it->first == "9");

        std_string_pair_vector vec({ string_pair("8", "8"), string_pair("9", "9"), string_pair("10", "10"), string_pair("11", "11"), string_pair("12", "12") });
        aa.insert(vec.begin(), vec.end());
        success &= aa.size() == 12;

        aa.insert({ string_pair("11", "11"), string_pair("12", "12"), string_pair("13", "13"), string_pair("14", "14"), string_pair("15", "15") });
        success &= aa.size() == 15;
    }

    {
        _string_map aa(yy);

        _iterator it = aa.erase(aa.cbegin());
        success &= (aa.size() == def_insert_count - 1 && it->first == number_to_string(1));

        it = aa.erase(aa.cend());
        success &= (aa.size() == def_insert_count - 1 && it == aa.end());

        size_t ret = aa.erase(number_to_string(100));
        success &= (aa.size() == def_insert_count - 2 && ret == 1);

        ret = aa.erase(number_to_string(def_insert_count));
        success &= (aa.size() == def_insert_count - 2 && ret == 0);

        it = aa.erase(aa.find(number_to_string(200)), aa.find(number_to_string(300)));
        success &= (aa.size() == def_insert_count - 102 && it->first == number_to_string(300));
        success &= (--it)->first == number_to_string(199);

        it = aa.erase(aa.find(number_to_string(def_insert_count - 100)), aa.cend());
        success &= (aa.size() == def_insert_count - 202 && it == aa.end());
        success &= (--it)->first == number_to_string(def_insert_count - 101);

        aa.clear();
        success &= aa.size() == 0;
    }

    {
        _string_map aa(yy);

        success &= (aa.count(number_to_string(100)) == 1 &&
                    aa.count(number_to_string(def_insert_count)) == 0);

        _iterator it = aa.find(number_to_string(200));
        success &= it->first == number_to_string(200);

        it = aa.find(number_to_string(def_insert_count + 100));
        success &= it == aa.end();

        aa.erase(number_to_string(0));
        it = aa.lower_bound(number_to_string(0));
        success &= it->first == number_to_string(1);

        it = aa.lower_bound(number_to_string(def_insert_count - 1));
        success &= it->first == number_to_string(def_insert_count - 1);

        it = aa.upper_bound(number_to_string(300));
        success &= it->first == number_to_string(301);

        it = aa.upper_bound(number_to_string(def_insert_count - 1));
        success &= it == aa.end();

        auto rp = aa.equal_range(number_to_string(400));
        success &= (rp.first->first == number_to_string(400) &&
                    rp.second->first == number_to_string(401));

        rp = aa.equal_range(number_to_string(def_insert_count - 1));
        success &= (rp.first->first == number_to_string(def_insert_count - 1) &&
                    rp.second == aa.end());
    }

    {
        _string_map aa({ string_pair("1", "1"), string_pair("2", "2"), string_pair("3", "3"), string_pair("4", "4"), string_pair("5", "5") });

        std::string key("2");
        success &= aa[key] == "2";

        aa[key] = "200";
        success &= (aa.size() == 5 && aa.find("2")->second == "200");

        key = "6";
        aa[key] = "600";
        success &= (aa.size() == 6 && aa.find("6")->second == "600");

        success &= aa["3"] == "3";

        aa["3"] = "300";
        success &= (aa.size() == 6 && aa.find("3")->second == "300");

        aa["7"] = "700";
        success &= (aa.size() == 7 && aa.find("7")->second == "700");

        key = "4";
        success &= aa.at("4") == "4";

        aa[key] = "400";
        success &= (aa.size() == 7 && aa.find("4")->second == "400");

        bool is_throw = false;
        try
        {
            aa.at("8") = "800";
        }
        catch(...)
        {
            is_throw = true;
        }
        success &= is_throw;
    }

    std::printf("%s\n", success? "passed" : "error");
}

void interface_test()
{
    std::printf("xxfl_int_set: ");
    int_set_interface_test<xxfl_int_set>();

    std::printf("xxfl_string_set: ");
    string_set_interface_test<xxfl_string_set>();

    std::printf("xxfl_int_map: ");
    int_map_interface_test<xxfl_int_map>();

    std::printf("xxfl_string_map: ");
    string_map_interface_test<xxfl_string_map>();
}
