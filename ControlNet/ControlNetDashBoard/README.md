# ControlNet Dashboard

A full-stack web application for monitoring and managing keystroke tracking devices with real-time analytics, device management, and data visualization capabilities.

## 📋 Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Configuration](#configuration)
- [API Endpoints](#api-endpoints)
- [Development](#development)
- [Deployment](#deployment)
- [Contributing](#contributing)
- [License](#license)

## 🌟 Overview

ControlNet Dashboard is a comprehensive monitoring solution that allows users to track keystroke sequences, manage multiple devices, view real-time statistics, and download client applications. The platform features user authentication, device management, activity visualization, and downloadable screenshots.

## 🏗️ System Architecture

<img width="1854" height="1114" alt="Interactive Architecture Diagram" src="https://github.com/user-attachments/assets/22064804-9315-4869-906d-e2a38e8318df" />

## ✨ Features

### 🔐 Authentication & User Management
- User registration and login
- Firebase-based authentication
- Secure session management
- User profile management

### 📱 Device Management
- Add and register new devices
- View all connected devices
- Device status monitoring
- Device-specific statistics
- Remove devices

### ⌨️ Keystroke Tracking
- Real-time keystroke sequence logging
- Sequence history by device
- Date-based sequence filtering
- Detailed sequence information

### 📊 Analytics & Visualization
- Activity charts and graphs
- Device usage statistics
- Custom calendar view
- Daily/weekly/monthly trends
- Real-time data updates

### 🖼️ Screenshots Gallery
- View captured screenshots
- Screenshot timeline
- Download screenshots
- Date-based filtering

### 📥 Client Application
- Download client software
- Client installation guide
- Version information

## 🛠️ Tech Stack

### Frontend
- **Framework**: React 19.1.1
- **Build Tool**: Vite 7.1.6
- **Routing**: React Router DOM 7.9.1
- **Styling**: Tailwind CSS 3.4.17
- **HTTP Client**: Axios 1.12.2
- **Charts**: Recharts 3.2.1
- **Icons**: Lucide React 0.544.0
- **Authentication**: Firebase 12.3.0

### Backend
- **Runtime**: Node.js
- **Framework**: Express.js 5.1.0
- **Database**: Firebase Admin SDK 13.5.0
- **CORS**: cors 2.8.5
- **Environment**: dotenv 17.2.2
- **Dev Tools**: Nodemon 3.1.10

### Development Tools
- **Package Manager**: npm
- **Process Manager**: Concurrently 8.2.2
- **Linting**: ESLint 9.35.0
- **CSS Processing**: PostCSS 8.5.6, Autoprefixer 10.4.21

## 📁 Project Structure

```
ControlNetDashBoard/
├── front/                      # React frontend application
│   ├── src/
│   │   ├── components/        # React components
│   │   │   ├── Auth/         # Authentication components
│   │   │   └── Dashboard/    # Dashboard components
│   │   ├── config/           # Frontend configuration
│   │   ├── context/          # React Context providers
│   │   ├── services/         # API service layer
│   │   ├── App.jsx           # Root component
│   │   └── main.jsx          # Entry point
│   ├── public/               # Static assets
│   ├── index.html            # HTML template
│   ├── vite.config.js        # Vite configuration
│   ├── tailwind.config.js    # Tailwind configuration
│   └── package.json          # Frontend dependencies
│
├── back/                      # Express backend application
│   ├── routes/               # API route handlers
│   │   ├── auth.js          # Authentication routes
│   │   ├── devices.js       # Device management routes
│   │   ├── sequences.js     # Keystroke sequence routes
│   │   ├── stats.js         # Statistics routes
│   │   └── download.js      # Download routes
│   ├── services/            # Business logic services
│   │   └── firestore.js     # Firestore database service
│   ├── middleware/          # Express middleware
│   │   └── index.js         # Logger & error handler
│   ├── config/              # Backend configuration
│   │   └── firebase.js      # Firebase admin config
│   ├── models/              # Data models
│   │   └── data.js          # Data structures
│   ├── downloads/           # Client application files
│   ├── server.js            # Server entry point
│   ├── secrets.json.example # Example secrets file
│   └── package.json         # Backend dependencies
│
├── firestore.rules           # Firestore security rules
├── package.json              # Root package configuration
└── README.md                 # This file
```

## 🚀 Getting Started

### Prerequisites

- Node.js (v16 or higher)
- npm (v7 or higher)
- Firebase account and project
- Git

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/reuven-itzhakov/ControlNetDashBoard.git
   cd ControlNetDashBoard
   ```

2. **Install all dependencies**
   ```bash
   npm run install:all
   ```
   
   Or install manually:
   ```bash
   # Install root dependencies
   npm install

   # Install frontend dependencies
   cd front
   npm install

   # Install backend dependencies
   cd ../back
   npm install
   ```

3. **Configure Firebase**
   
   a. Create a Firebase project at [Firebase Console](https://console.firebase.google.com/)
   
   b. Enable Firestore Database and Cloud Storage
   
   c. Enable Authentication (Email/Password)
   
   d. Download service account key and save as `back/secrets.json`

4. **Configure Frontend Firebase**
   
   Update `front/src/config/firebase.js` with your Firebase config:
   ```javascript
   const firebaseConfig = {
     apiKey: "your-api-key",
     authDomain: "your-auth-domain",
     projectId: "your-project-id",
     storageBucket: "your-storage-bucket",
     messagingSenderId: "your-messaging-sender-id",
     appId: "your-app-id"
   };
   ```

5. **Configure Backend Environment**
   
   Create `back/.env` file:
   ```env
   PORT=5000
   NODE_ENV=development
   ```

6. **Start the development servers**
   ```bash
   # From root directory
   npm run dev
   ```
   
   This will start:
   - Frontend: http://localhost:5173
   - Backend: http://localhost:5000

## ⚙️ Configuration

### Firebase Setup

1. **Firestore Database Rules**: Deploy `firestore.rules` to your Firebase project
2. **Service Account**: Place your Firebase service account JSON in `back/secrets.json`
3. **Frontend Config**: Update Firebase config in `front/src/config/firebase.js`

### Backend Configuration

The backend uses `secrets.json` for Firebase Admin SDK initialization. Example structure:
```json
{
  "type": "service_account",
  "project_id": "your-project-id",
  "private_key_id": "...",
  "private_key": "...",
  "client_email": "...",
  "client_id": "...",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "...",
  "client_x509_cert_url": "..."
}
```

## 📡 API Endpoints

### Authentication
- `POST /api/auth/register` - Register new user

### Devices
- `GET /api/devices` - Get all devices for a user
- `GET /api/devices/:deviceId` - Get specific device details
- `POST /api/devices` - Add new device
- `PUT /api/devices/:deviceId` - Update device information
- `DELETE /api/devices/:deviceId` - Remove device

### Sequences
- `GET /api/sequences` - Get all sequences for a user
- `GET /api/sequences/:deviceId` - Get sequences for specific device
- `POST /api/sequences` - Log new keystroke sequence

### Statistics
- `GET /api/stats/:deviceId` - Get device statistics

### Downloads
- `GET /api/download/client` - Download client application
- `GET /api/download/info` - Get download information

### Health Check
- `GET /api/health` - Server health status

## 💻 Development

### Available Scripts

#### Root Level
```bash
npm run dev              # Start both frontend and backend
npm run dev:frontend     # Start only frontend
npm run dev:backend      # Start only backend
npm run build            # Build frontend for production
npm run install:all      # Install all dependencies
npm run clean            # Clean all node_modules
```

#### Frontend (in `front/` directory)
```bash
npm run dev              # Start development server (Vite)
npm run build            # Build for production
npm run lint             # Run ESLint
npm run preview          # Preview production build
```

#### Backend (in `back/` directory)
```bash
npm run dev              # Start with nodemon (auto-restart)
npm start                # Start production server
```

### Development Guidelines

#### Frontend
- Use Tailwind CSS utility classes for styling
- Follow React best practices and hooks patterns
- Maintain component modularity and reusability
- Use ES6+ features and modern JavaScript syntax
- Implement proper error handling
- Use React Context for global state management

#### Backend
- Follow RESTful API conventions
- Use middleware for cross-cutting concerns
- Implement proper error handling
- Store sensitive configuration in environment variables
- Use async/await for asynchronous operations
- Validate input data
- Return consistent response formats

### Code Style
- Use meaningful variable and function names
- Write clean, self-documenting code
- Add comments for complex logic
- Follow consistent formatting (use ESLint)
- Keep functions small and focused

## 🌐 Deployment

### Frontend Deployment (Vercel/Netlify)

1. Build the frontend:
   ```bash
   cd front
   npm run build
   ```

2. Deploy the `front/dist` directory to your hosting platform

3. Update environment variables with production Firebase config

### Backend Deployment (Heroku/Railway/Render)

1. Set environment variables on your hosting platform:
   - `PORT` (usually set automatically)
   - Firebase service account details

2. Deploy the `back/` directory

3. Update CORS settings in `server.js` to allow your frontend domain

### Environment Variables

Make sure to set the following in production:
- Firebase configuration (frontend)
- Firebase Admin SDK credentials (backend)
- API URLs
- CORS allowed origins

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Coding Standards
- Follow the existing code style
- Write meaningful commit messages
- Add tests for new features
- Update documentation as needed
- Ensure all tests pass before submitting

## 📄 License

This project is licensed under the ISC License.

## 👥 Authors

- **Reuven Itzhakov** - *Initial work* - [reuven-itzhakov](https://github.com/reuven-itzhakov)

## 🙏 Acknowledgments

- Firebase for backend services
- React team for the amazing framework
- Tailwind CSS for the utility-first CSS framework
- Vite for lightning-fast build tool
- All contributors and users of this project

## 📞 Support

For support, please open an issue on the GitHub repository or contact the development team.

---

**Built with ❤️ using React, Express, and Firebase**
