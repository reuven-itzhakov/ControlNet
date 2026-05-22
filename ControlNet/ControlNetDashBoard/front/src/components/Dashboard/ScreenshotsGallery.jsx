import React, { useState } from 'react';
import { Camera, Download, X, Clock } from 'lucide-react';

const ScreenshotsGallery = ({ screenshots }) => {
  const [selectedImage, setSelectedImage] = useState(null);

  // Download screenshot
  const downloadScreenshot = (screenshot, e) => {
    e.stopPropagation();
    
    const link = document.createElement('a');
    link.href = `data:image/png;base64,${screenshot.imageData}`;
    const timestamp = new Date(screenshot.timestamp).toISOString().replace(/[:.]/g, '-');
    link.download = `screenshot-${timestamp}.png`;
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  // Format time from timestamp
  const formatTime = (timestamp) => {
    return new Date(timestamp).toLocaleTimeString([], {
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit'
    });
  };

  if (screenshots.length === 0) {
    return (
      <div className="neon-card bg-gray-800/50 rounded-xl p-12 text-center">
        <Camera className="w-16 h-16 text-gray-500 mx-auto mb-4" />
        <p className="text-gray-400 text-lg">No screenshots for selected date</p>
        <p className="text-gray-500 text-sm mt-2">Screenshots will appear here when available</p>
      </div>
    );
  }

  return (
    <>
      <div className="neon-card bg-gray-800/50 rounded-xl p-6">
        <div className="flex items-center justify-between mb-4">
          <h4 className="text-lg font-semibold text-white">
            Screenshots ({screenshots.length})
          </h4>
        </div>
        
        {/* Screenshots Grid */}
        <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 gap-4 max-h-[600px] overflow-y-auto custom-scrollbar">
          {screenshots.map((screenshot) => (
            <div
              key={screenshot.id}
              className="group relative aspect-video bg-gray-700/30 rounded-lg overflow-hidden border border-gray-600/30 hover:border-cyan-500/50 transition-all cursor-pointer"
              onClick={() => setSelectedImage(screenshot)}
            >
              {/* Thumbnail */}
              <img
                src={`data:image/png;base64,${screenshot.imageData}`}
                alt={`Screenshot at ${formatTime(screenshot.timestamp)}`}
                className="w-full h-full object-cover"
                loading="lazy"
              />
              
              {/* Overlay on hover */}
              <div className="absolute inset-0 bg-black/60 opacity-0 group-hover:opacity-100 transition-opacity flex items-center justify-center">
                <div className="text-center">
                  <Clock className="w-8 h-8 text-cyan-400 mx-auto mb-2" />
                  <p className="text-white text-sm font-medium">{formatTime(screenshot.timestamp)}</p>
                </div>
              </div>
              
              {/* Timestamp badge */}
              <div className="absolute bottom-2 left-2 bg-black/70 backdrop-blur-sm px-2 py-1 rounded text-xs text-white">
                {formatTime(screenshot.timestamp)}
              </div>
              
              {/* Download button */}
              <button
                onClick={(e) => downloadScreenshot(screenshot, e)}
                className="absolute top-2 right-2 p-1.5 bg-black/70 backdrop-blur-sm rounded-lg opacity-0 group-hover:opacity-100 transition-opacity hover:bg-cyan-500/20"
                title="Download screenshot"
              >
                <Download className="w-4 h-4 text-cyan-400" />
              </button>
            </div>
          ))}
        </div>
      </div>

      {/* Lightbox Modal */}
      {selectedImage && (
        <div
          className="fixed inset-0 bg-black/80 backdrop-blur-sm z-50 flex items-center justify-center p-4"
          onClick={() => setSelectedImage(null)}
        >
          <div className="relative max-w-7xl max-h-[90vh] w-full">
            {/* Close button */}
            <button
              onClick={() => setSelectedImage(null)}
              className="absolute -top-12 right-0 p-2 bg-gray-800/90 hover:bg-red-500/20 rounded-lg transition-all group"
            >
              <X className="w-6 h-6 text-gray-400 group-hover:text-red-400" />
            </button>
            
            {/* Download button */}
            <button
              onClick={(e) => {
                downloadScreenshot(selectedImage, e);
                setSelectedImage(null);
              }}
              className="absolute -top-12 right-14 p-2 bg-gray-800/90 hover:bg-cyan-500/20 rounded-lg transition-all group"
            >
              <Download className="w-6 h-6 text-gray-400 group-hover:text-cyan-400" />
            </button>
            
            {/* Image */}
            <img
              src={`data:image/png;base64,${selectedImage.imageData}`}
              alt={`Screenshot at ${formatTime(selectedImage.timestamp)}`}
              className="w-full h-full object-contain rounded-xl"
              onClick={(e) => e.stopPropagation()}
            />
            
            {/* Timestamp */}
            <div className="absolute bottom-4 left-4 bg-black/70 backdrop-blur-sm px-4 py-2 rounded-lg">
              <p className="text-white font-medium">
                {new Date(selectedImage.timestamp).toLocaleString()}
              </p>
            </div>
          </div>
        </div>
      )}
    </>
  );
};

export default ScreenshotsGallery;
