#include "type_dispatching.h"
#include "testing.h"
#include <deque>

using container::any_of;
using container::static_any_of;
using container::dispatch_queue;
using container::static_dispatch_queue;

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
	any1 a;
	a_visitor v;
	a.accept(v);
}

TEST(any_of_visitor_gets_correct_call)
{
	any1 a;
	a_visitor v;

	a = 33.4f;
	a.accept(v);

	a = 1;
	a.accept(v);

	a = event3{778, 0};
	a.accept(v);
}

using any2 = static_any_of<int, float, event3>;
struct b_visitor final {
	void visit(const int& v) { LOGGER << __PRETTY_FUNCTION__ << PRINT(v) << std::endl; }
	void visit(const float& v) { LOGGER << __PRETTY_FUNCTION__ << PRINT(v) << std::endl; }
	void visit(const event3& v) { LOGGER << __PRETTY_FUNCTION__ << PRINT(v.with_attributes[0]) << std::endl; }
};

TEST(static_any_of_visitor_wont_get_anything_if_unset)
{
	any2 a;
	b_visitor v;
	a.accept(v);
}

TEST(static_any_of_visitor_gets_correct_call)
{
	any2 a;
	b_visitor v;

	a = 33.4f;
	a.accept(v);

	a = 1;
	a.accept(v);

	a = event3{778, 0};
	a.accept(v);
}

template<typename T>
struct queue {
	std::deque<T> q;

	T* acquire() { q.emplace_back(); return &q.back(); }
	void push(T*) { }
	T* pop() { return &q.front(); }
	void release(T*) { q.pop_front(); }
};

using event_dispatcher = dispatch_queue<queue, event1, event2, event3>;

struct event_handler : event_dispatcher::subscriber {
	 void visit(const event1&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event2&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event3&) override { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
};

TEST(dispatch_queue_works)
{
	queue<event_dispatcher::element_type> q;
	event_dispatcher d{q};
	event_handler h;

	d.post<event1>();
	d.post<event2>();
	d.post<event1>();
	d.post<event3>();

	d.dispatch(h);
	d.dispatch(h);
	d.dispatch(h);
	d.dispatch(h);
}

using static_dispatcher = static_dispatch_queue<queue, event1, event2, event3>;

struct static_handler1 {
	 void visit(const event1&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event2&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event3&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
};

struct static_handler2 {
	 void visit(const event1&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event2&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
	 void visit(const event3&) { LOGGER << __PRETTY_FUNCTION__ << std::endl; }
};

TEST(static_dispatch_queue_works)
{
	queue<static_dispatcher::element_type> q;
	static_dispatcher d{q};
	static_handler1 h1;
	static_handler2 h2;

	d.post<event1>();
	d.post<event2>();
	d.post<event1>();
	d.post<event3>();

	d.dispatch(h1, h2);
	d.dispatch(h1, h2);
	d.dispatch(h1, h2);
	d.dispatch(h1, h2);
}

int main()
{
	return test::registry::run_all("runtime_dispatch") ? EXIT_SUCCESS : EXIT_FAILURE;
}
