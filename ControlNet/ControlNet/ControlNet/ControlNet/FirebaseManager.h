#pragma once
#include "firebase/app.h"
#include "firebase/auth.h"
#include "firebase/database.h"

// Firebase global variables
extern firebase::App* g_firebaseApp;
extern firebase::auth::Auth* g_firebaseAuth;
extern firebase::database::Database* g_firebaseDatabase;

// Firebase initialization
void InitializeFirebase();