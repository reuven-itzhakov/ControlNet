This workspace contains a full-stack application with a React.js frontend and Express.js backend.

## Project Structure

- `front/` - React.js frontend with Vite and Tailwind CSS
- `back/` - Express.js backend with CORS and environment variables
- Root level package.json with scripts to manage both frontend and backend

## Project Setup Complete

- [x] Verify that the copilot-instructions.md file in the .github directory is created.
- [x] Clarify Project Requirements - Full-stack: React.js + Tailwind CSS + Vite + Express.js
- [x] Scaffold the Project - Frontend and backend structured in separate folders
- [x] Customize the Project - Tailwind CSS integrated, Express.js server with sample endpoints
- [x] Install Required Extensions - No additional extensions needed
- [x] Compile the Project - Dependencies installed for both frontend and backend
- [x] Create and Run Task - Development scripts created for full-stack development
- [x] Launch the Project - Both servers can be started with `npm run dev`
- [x] Ensure Documentation is Complete - README.md updated with full-stack information

## Development Guidelines

### Frontend (React + Vite + Tailwind)
- Use Tailwind CSS utility classes for styling
- Follow React best practices and hooks patterns
- Maintain component modularity and reusability
- Use ES6+ features and modern JavaScript syntax
- Frontend runs on http://localhost:5173/

### Backend (Express.js)
- Follow RESTful API conventions
- Use middleware for CORS, body parsing, and error handling
- Store configuration in environment variables
- Backend runs on http://localhost:5000/
- Use nodemon for development auto-restart

## Available Commands

### Root Level
- `npm run dev` - Start both frontend and backend
- `npm run dev:frontend` - Start only frontend
- `npm run dev:backend` - Start only backend
- `npm run build` - Build frontend for production
- `npm run install:all` - Install all dependencies

### Frontend (in `front/` directory)
- `npm run dev` - Start development server
- `npm run build` - Build for production
- `npm run lint` - Run ESLint

### Backend (in `back/` directory)
- `npm run dev` - Start with nodemon
- `npm start` - Start production server