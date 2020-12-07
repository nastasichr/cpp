#include "type_dispatching.h"
#include "testing.h"
#include <deque>

using container::any_of;
using container::type_dispatcher;

struct event1 {};
struct event2 {};
struct event3 {
	int with_attributes[1024];
};


using any1 = any_of<int, float, event3>;
struct a_visitor : any1::visitor {
	void visit(const int& v) override { LOGGER << __PRETTY_FUNCTION__ << PRINT(v) << std::endl; }
	void visit(const float& v) override { LOGGER << __PRETTY_FUNCTION__ << PRINT(v) << std::endl; }
	void visit(const event3& v) override { LOGGER << __PRETTY_FUNCTION__ << PRINT(v.with_attributes[0]) << std::endl; }
};

TEST(any_of_visitor_wont_get_anything_if_unset)
{
	any1 a1;
	a_visitor v1;
	a1.accept(v1);
}

TEST(any_of_visitor_gets_correct_call)
{
	any1 a1;
	a_visitor v1;

	a1 = 33.4f;
	a1.accept(v1);

	a1 = 1;
	a1.accept(v1);

	a1 = event3{778, 0};
	a1.accept(v1);
}

template<typename T>
struct queue {
	std::deque<T> q;

	T* acquire() { q.emplace_back(); return &q.back(); }
	void push(T*) { }
	T* pop() { return &q.front(); }
	void release(T*) { q.pop_front(); }
};

using event_dispatcher = type_dispatcher<queue, 1, event1, event2, event3>;

struct event_handler : event_dispatcher::subscriber {
	 void visit(const event1&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event2&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event3&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
};

TEST(type_dispatcher_works)
{
	queue<event_dispatcher::element_type> q;
	event_dispatcher d{q};
	event_handler h;

	d.subscribe(h);

	d.post<event1>();
	d.post<event2>();
	d.post<event1>();
	d.post<event3>();

	d.dispatch();
	d.dispatch();
	d.dispatch();
	d.dispatch();
}

int main()
{
	return test::registry::run_all("runtime_dispatch") ? EXIT_SUCCESS : EXIT_FAILURE;
}
