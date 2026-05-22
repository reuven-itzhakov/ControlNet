import React from 'react';
import { Routes, Route } from 'react-router-dom';
import DashboardLayout from './DashboardLayout';
import Overview from './Overview';
import DeviceDetail from './DeviceDetail';
import DevicesList from './DevicesList';

const Dashboard = () => {
  return (
    <DashboardLayout>
      <Routes>
        <Route path="/" element={<Overview />} />
        <Route path="/devices" element={<DevicesList />} />
        {/* <Route path="/device/:deviceId" element={<DeviceDetail />} /> */}
      </Routes>
    </DashboardLayout>
  );
};

export default Dashboard;