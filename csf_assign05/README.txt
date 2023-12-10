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

 These critical sections were synchronized with the use of mutex locks, which were abstracted
 by use of the Guard object. Locks prevent multiple threads from accessing a critical section
 simultaneously, and thus are used to prevent accesses to the shared data structures in this program.
 Additionally, semaphores were used with the MessageQueue structure
 as a way to indicate to receivers when a sender has broadcast messages within a room, and when
 those messages were available to be read.