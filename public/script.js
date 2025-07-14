// Zod-style validation schema
const ContactSchema = {
  name: {
    required: true,
    minLength: 2,
    maxLength: 50,
    pattern: /^[a-zA-Z\s'-]+$/,
    message:
      "Name must be 2-50 characters and contain only letters, spaces, hyphens, and apostrophes",
  },
  email: {
    required: true,
    pattern: /^[^\s@]+@[^\s@]+\.[^\s@]+$/,
    message: "Please enter a valid email address",
  },
  subject: {
    required: true,
    minLength: 5,
    maxLength: 100,
    message: "Subject must be between 5 and 100 characters",
  },
  message: {
    required: true,
    minLength: 10,
    maxLength: 1000,
    message: "Message must be between 10 and 1000 characters",
  },
};

// Validation function
function validateField(fieldName, value) {
  const schema = ContactSchema[fieldName];
  const errors = [];

  if (schema.required && (!value || value.trim() === "")) {
    errors.push(
      `${fieldName.charAt(0).toUpperCase() + fieldName.slice(1)} is required`,
    );
    return errors;
  }

  if (value && value.trim() !== "") {
    const trimmedValue = value.trim();

    if (schema.minLength && trimmedValue.length < schema.minLength) {
      errors.push(
        schema.message ||
          `${fieldName} must be at least ${schema.minLength} characters`,
      );
    }

    if (schema.maxLength && trimmedValue.length > schema.maxLength) {
      errors.push(
        schema.message ||
          `${fieldName} must be no more than ${schema.maxLength} characters`,
      );
    }

    if (schema.pattern && !schema.pattern.test(trimmedValue)) {
      errors.push(schema.message || `${fieldName} format is invalid`);
    }
  }

  return errors;
}

// Show error message
function showError(fieldName, message) {
  const formGroup = document.getElementById(fieldName).closest(".form-group");
  const errorElement = document.getElementById(`${fieldName}-error`);

  formGroup.classList.add("error");
  errorElement.textContent = message;
  errorElement.classList.add("show");
}

// Clear error message
function clearError(fieldName) {
  const formGroup = document.getElementById(fieldName).closest(".form-group");
  const errorElement = document.getElementById(`${fieldName}-error`);

  formGroup.classList.remove("error");
  errorElement.textContent = "";
  errorElement.classList.remove("show");
}

// Validate form data
function validateForm(formData) {
  const errors = {};
  let isValid = true;

  // Clear all previous errors
  Object.keys(ContactSchema).forEach((field) => {
    clearError(field);
  });

  // Validate each field
  Object.keys(ContactSchema).forEach((field) => {
    const fieldErrors = validateField(field, formData[field]);
    if (fieldErrors.length > 0) {
      errors[field] = fieldErrors[0]; // Show first error
      showError(field, fieldErrors[0]);
      isValid = false;
    }
  });

  return { isValid, errors };
}

// Handle form submission
async function handleSubmit(event) {
  event.preventDefault();

  const form = event.target;
  const submitButton = form.querySelector(".submit-btn");
  const formData = new FormData(form);

  // Convert FormData to object
  const data = {};
  formData.forEach((value, key) => {
    data[key] = value;
  });

  // Validate form
  const validation = validateForm(data);

  if (!validation.isValid) {
    return;
  }

  // Disable submit button
  submitButton.disabled = true;
  submitButton.textContent = "Sending...";

  try {
    // Simulate API call (replace with actual endpoint)
    await new Promise((resolve) => setTimeout(resolve, 1000));

    // Success feedback
    alert("Message sent successfully! I'll get back to you soon.");
    form.reset();
  } catch (error) {
    console.error("Error sending message:", error);
    alert("Sorry, there was an error sending your message. Please try again.");
  } finally {
    // Re-enable submit button
    submitButton.disabled = false;
    submitButton.textContent = "Send Message";
  }
}

// Real-time validation
function setupRealTimeValidation() {
  Object.keys(ContactSchema).forEach((fieldName) => {
    const field = document.getElementById(fieldName);

    field.addEventListener("blur", () => {
      const errors = validateField(fieldName, field.value);
      if (errors.length > 0) {
        showError(fieldName, errors[0]);
      } else {
        clearError(fieldName);
      }
    });

    field.addEventListener("input", () => {
      // Clear error on input if field was previously invalid
      const formGroup = field.closest(".form-group");
      if (formGroup.classList.contains("error")) {
        const errors = validateField(fieldName, field.value);
        if (errors.length === 0) {
          clearError(fieldName);
        }
      }
    });
  });
}

// Initialize form
document.addEventListener("DOMContentLoaded", () => {
  const form = document.getElementById("contact-form");
  form.addEventListener("submit", handleSubmit);
  setupRealTimeValidation();

  console.log("Contact form initialized with Zod-style validation");
});
