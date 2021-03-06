#include <stdio.h>
#include <stdlib.h>
#include "tool/testfw/unittest.h"
#include "common/errno.h"
#include "test/unit/fixture/session.fixture.h"
#include "osd/containers/name/container.h"


using namespace osd::containers::common;
	
SUITE(ContainersNameContainer)
{
	TEST_FIXTURE(SessionFixture, TestInsertLookup)
	{
		osd::common::ObjectId           oid;
		volatile char*                  buffer = (volatile char*) malloc(sizeof(NameContainer::Object<Session>));
		NameContainer::Object<Session>* name_container = NameContainer::Object<Session>::Make(session, buffer);
		
		CHECK(name_container->Insert(session, ".", osd::common::ObjectId(1)) == 0);
		CHECK(name_container->Insert(session, "..", osd::common::ObjectId(2)) == 0);
		CHECK(name_container->Insert(session, "foo", osd::common::ObjectId(3)) == 0);
		CHECK(name_container->Insert(session, "bar", osd::common::ObjectId(4)) == 0);
		CHECK(name_container->Find(session, ".", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(1));
		CHECK(name_container->Find(session, "..", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(2));
		CHECK(name_container->Find(session, "foo", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(3));
		CHECK(name_container->Find(session, "bar", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(4));
	}

	TEST_FIXTURE(SessionFixture, TestEraseDot)
	{
		osd::common::ObjectId           oid;
		volatile char*                  buffer = (volatile char*) malloc(sizeof(NameContainer::Object<Session>));
		NameContainer::Object<Session>* name_container = NameContainer::Object<Session>::Make(session, buffer);
		
		CHECK(name_container->Insert(session, ".", osd::common::ObjectId(2)) == 0);
		CHECK(name_container->Insert(session, "..", osd::common::ObjectId(1)) == 0);
		
		CHECK(name_container->Find(session, ".", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(2));
		CHECK(name_container->Find(session, "..", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(1));
		
		CHECK(name_container->Erase(session, ".") == 0);
		CHECK(name_container->Find(session, ".", &oid) == -E_EXIST);
	
		CHECK(name_container->Erase(session, "..") == 0);
		CHECK(name_container->Find(session, "..", &oid) == -E_EXIST);
	}

	TEST_FIXTURE(SessionFixture, TestErase)
	{
		osd::common::ObjectId           oid;
		volatile char*                  buffer = (volatile char*) malloc(sizeof(NameContainer::Object<Session>));
		NameContainer::Object<Session>* name_container = NameContainer::Object<Session>::Make(session, buffer);
		
		CHECK(name_container->Insert(session, ".", osd::common::ObjectId(1)) == 0);
		CHECK(name_container->Insert(session, "..", osd::common::ObjectId(2)) == 0);
		CHECK(name_container->Insert(session, "foo", osd::common::ObjectId(3)) == 0);
		CHECK(name_container->Insert(session, "bar", osd::common::ObjectId(4)) == 0);
		CHECK(name_container->Insert(session, "doc", osd::common::ObjectId(12)) == 0);
		
		CHECK(name_container->Find(session, ".", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(1));
		CHECK(name_container->Find(session, "..", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(2));
		CHECK(name_container->Find(session, "foo", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(3));
		CHECK(name_container->Find(session, "bar", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(4));
		CHECK(name_container->Find(session, "doc", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(12));
	
		CHECK(name_container->Erase(session, "foo") == 0);
		CHECK(name_container->Find(session, "foo", &oid) != 0);
		
		CHECK(name_container->Erase(session, "bar") == 0);
		CHECK(name_container->Find(session, "bar", &oid) != 0);
	
		CHECK(name_container->Insert(session, "foo", 13) == 0);
		CHECK(name_container->Find(session, "foo", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(13));
		CHECK(name_container->Insert(session, "bar", 14) == 0);
		CHECK(name_container->Find(session, "bar", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(14));
		
		CHECK(name_container->Erase(session, "doc") == 0);
		CHECK(name_container->Find(session, "doc", &oid) != 0);
	
		CHECK(name_container->Erase(session, "foo") == 0);
		CHECK(name_container->Find(session, "foo", &oid) != 0);
		CHECK(name_container->Erase(session, "bar") == 0);
		CHECK(name_container->Find(session, "bar", &oid) != 0);
	}


	TEST_FIXTURE(SessionFixture, TestInsertOverwrite)
	{
		osd::common::ObjectId           oid;
		volatile char*                  buffer = (volatile char*) malloc(sizeof(NameContainer::Object<Session>));
		NameContainer::Object<Session>* name_container = NameContainer::Object<Session>::Make(session, buffer);
		
		CHECK(name_container->Insert(session, ".", osd::common::ObjectId(1)) == 0);
		CHECK(name_container->Insert(session, "..", osd::common::ObjectId(2)) == 0);
		CHECK(name_container->Insert(session, "foo", osd::common::ObjectId(3)) == 0);
		CHECK(name_container->Insert(session, "bar", osd::common::ObjectId(4)) == 0);
		
		CHECK(name_container->Find(session, ".", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(1));
		CHECK(name_container->Find(session, "..", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(2));
		CHECK(name_container->Find(session, "foo", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(3));
		CHECK(name_container->Find(session, "bar", &oid) == 0);
		CHECK(oid == osd::common::ObjectId(4));
	
		CHECK(name_container->Insert(session, "foo", osd::common::ObjectId(13)) != 0);
	}

}
