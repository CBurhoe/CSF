All code implemented by Casey Burhoe

Eventually your report about how you implemented thread synchronization
in the server should go here

Synchronization report:
The critical sections in my code are in the functions that attempt to change one
 of the shared data structures. These are the server's m_rooms RoomMap, the MessageQueue's
 deque of Messages, and the UserSet in each Room. The functions that alter these
 data structures, and thus the critical sections in the program, are the
 Server::find_or_create_room(), MessageQueue::enqueue() and MessageQueue::dequeue(),
 and Room::add_member() and Room::remove_member(). For each of these functions,
 the entire function scope was considered to be a critical section, with the exception
 of MessgaeQueue::dequeue().