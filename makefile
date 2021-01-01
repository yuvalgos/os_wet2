
CC = g++
CXXFLAGS = -g -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
CCLINK = $(CC) 
OBJS = account.o accounts_collection.o bank.o
RM = rm -f
# Creating the  executable
Bank: $(OBJS)
	$(CCLINK) -o Bank $(OBJS)

# Creating the object files
bank.o: bank.cpp account.h accounts_collection.h

account.o: account.cpp account.h

accounts_collection.o: accounts_collection.cpp accounts_collection.h account.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

