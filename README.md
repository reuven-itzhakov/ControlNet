# ControlNet Monorepo

This repository contains two related projects for device monitoring and a web dashboard for management and analytics.

## Subprojects
- **ControlNet (Desktop client)**: A Windows C++ monitoring and activity-tracking application with Firebase sync. See [ControlNet/ControlNet/README.md](ControlNet/ControlNet/README.md) for full details.
- **ControlNetDashBoard (Web dashboard)**: A full-stack React + Node.js dashboard for device management, analytics, and downloads. See [ControlNetDashBoard/README.md](ControlNetDashBoard/README.md) for full details.

## Quick Overview
- **Purpose**: Capture keystrokes, screenshots, and device telemetry on Windows clients and provide a web dashboard to view, manage, and analyze collected data.
- **Tech stack**: C++ (Windows, Firebase C++ SDK) for the client; React (Vite, Tailwind) frontend and Express backend (Firebase Admin) for the dashboard.

## Getting Started

Prerequisites (general):
- Windows 10/11 for the desktop client
- Visual Studio 2019+ to build the C++ client
- Node.js (v16+) and npm for the dashboard
- Firebase project for cloud services (Auth, Firestore, Storage)

1. Clone the repository:

```
git clone https://github.com/reuven-itzhakov/ControlNet.git
cd ControlNet
```

2. Follow each subproject's README to configure and run:
- Desktop client: follow the instructions in [ControlNet/ControlNet/README.md](ControlNet/ControlNet/README.md).
- Dashboard: follow the instructions in [ControlNetDashBoard/README.md](ControlNetDashBoard/README.md).

## Development Notes
- The desktop client stores logs and screenshots locally under a device GUID directory and synchronizes with Firebase when available.
- The dashboard uses Firebase Admin SDK for backend operations and expects a service account JSON in `ControlNetDashBoard/back/secrets.json` (see that README for details).

## Project Structure

- `ControlNet/ControlNet/` — C++ client source, build solution, and module files.
- `ControlNetDashBoard/front/` — React frontend (Vite + Tailwind).
- `ControlNetDashBoard/back/` — Express backend and API routes.

## Security & Legal
This software is intended for legitimate monitoring purposes only (parental control, authorized device management). Unauthorized use may violate laws. See the desktop README for the explicit legal notice and privacy guidance: [ControlNet/ControlNet/README.md](ControlNet/ControlNet/README.md).

## Contributing
See each subproject README for contribution guidance. General workflow:

```
# Work on a feature branch
git checkout -b feature/my-feature
git commit -m "Describe change"
git push origin feature/my-feature
```

## License
See the subproject READMEs for license and disclaimer information.

---
If you want, I can also:
- add a simple root-level dev script to start both dashboard and client build tasks (where applicable), or
- open a consolidated CONTRIBUTING.md.
