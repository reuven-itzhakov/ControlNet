// Authentication middleware (mock implementation)
const authMiddleware = (req, res, next) => {
  const token = req.headers.authorization;
  
  // In a real app, you would verify the JWT token here
  if (!token) {
    return res.status(401).json({
      success: false,
      error: 'Access token required'
    });
  }
  
  // For now, we're using mock authentication with headers
  // In production, you would decode the token and set req.user
  next();
};

// Error handling middleware
const errorHandler = (err, req, res, next) => {
  console.error(err.stack);
  
  // Default error
  let error = {
    message: err.message || 'Internal Server Error',
    status: err.status || 500
  };
  
  // Mongoose validation error
  if (err.name === 'ValidationError') {
    error.message = Object.values(err.errors).map(val => val.message).join(', ');
    error.status = 400;
  }
  
  // Mongoose duplicate key error
  if (err.code === 11000) {
    error.message = 'Duplicate field value entered';
    error.status = 400;
  }
  
  // Mongoose cast error
  if (err.name === 'CastError') {
    error.message = 'Resource not found';
    error.status = 404;
  }
  
  res.status(error.status).json({
    success: false,
    error: error.message,
    ...(process.env.NODE_ENV === 'development' && { stack: err.stack })
  });
};

// Request logging middleware
const logger = (req, res, next) => {
  const timestamp = new Date().toISOString();
  console.log(`[${timestamp}] ${req.method} ${req.path} - ${req.ip}`);
  next();
};

// Validation middleware factory
const validateRequest = (schema) => {
  return (req, res, next) => {
    const { error } = schema.validate(req.body);
    if (error) {
      return res.status(400).json({
        success: false,
        error: error.details[0].message
      });
    }
    next();
  };
};

module.exports = {
  authMiddleware,
  errorHandler,
  logger,
  validateRequest
};