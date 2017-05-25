#pragma once

#include <Box2D\Box2D.h>
#include <iostream>
using namespace std;

extern int PlayerLifes;

class ContactListener : public b2ContactListener
{
public:
	virtual void BeginContact(b2Contact* aContact)
	{
		b2Fixture* fixtureA = aContact->GetFixtureA();
		b2Fixture* fixtureB = aContact->GetFixtureB();

		string userDataA((char*)fixtureA->GetBody()->GetUserData());
		string userDataB((char*)fixtureB->GetBody()->GetUserData());

		cout << userDataA.c_str() << endl;
		cout << userDataB.c_str() << endl;

		if (userDataA == "brick")
		{
			fixtureA->GetBody()->SetUserData("delete");
		}
		if (userDataA == "floor")
		{
			PlayerLifes--;
		}
	}
};