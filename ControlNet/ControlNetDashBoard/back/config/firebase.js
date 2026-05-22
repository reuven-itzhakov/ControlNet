const admin = require('firebase-admin');

// Initialize Firebase Admin (singleton pattern)
let db = null;

function initializeFirebase() {
  if (db) {
    return db; // Already initialized
  }

  if (!admin.apps.length) {
    try {
      // Load secrets configuration
      const secrets = require('../secrets.json');
      const { firebase } = secrets;
      
      // Validate credentials
      if (!firebase.serviceAccount.private_key || 
          firebase.serviceAccount.private_key.includes('YOUR_PRIVATE_KEY_FROM_FIREBASE_CONSOLE') ||
          firebase.serviceAccount.client_email.includes('xxxxx')) {
        throw new Error('Invalid Firebase credentials in secrets.json');
      }
      
      admin.initializeApp({
        credential: admin.credential.cert(firebase.serviceAccount),
        databaseURL: firebase.databaseURL,
        projectId: firebase.projectId
      });
      
      console.log('✅ Firebase Admin SDK initialized with service account credentials from secrets.json');
    } catch (error) {
      console.error('❌ Firebase initialization failed:', error.message);
      
      // Fallback to basic initialization
      const projectId = process.env.FIREBASE_PROJECT_ID || 'controlnet-0';
      admin.initializeApp({
        projectId: projectId,
      });
      console.warn('⚠️ Using fallback Firebase configuration');
    }
  }

  db = admin.firestore();
  return db;
}

// Get Firestore instance
function getFirestore() {
  if (!db) {
    return initializeFirebase();
  }
  return db;
}

// Get Admin instance
function getAdmin() {
  if (!admin.apps.length) {
    initializeFirebase();
  }
  return admin;
}

module.exports = {
  initializeFirebase,
  getFirestore,
  getAdmin,
  admin
};
