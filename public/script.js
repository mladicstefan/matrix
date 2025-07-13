const countBtn = document.getElementById("countBtn");
const countDisplay = document.getElementById("countDisplay");
let count = 0;

countBtn.addEventListener("click", () => {
  count += 1;
  countDisplay.textContent = count;
});

const { z } = window;
const contactForm = document.getElementById("contactForm");
const formStatus = document.getElementById("formStatus");

const ContactSchema = z.object({
  name: z.string().min(1, "Name is required"),
  email: z.string().email("Invalid email"),
  msg: z.string().min(5, "Message must be at least 5 characters"),
});

contactForm.addEventListener("submit", (e) => {
  e.preventDefault();
  const formData = new FormData(contactForm);
  const data = Object.fromEntries(formData.entries());

  const result = ContactSchema.safeParse(data);

  if (result.success) {
    formStatus.textContent = "✅ Form is valid! (Pretend we sent it…)";
    formStatus.style.color = "green";
    contactForm.reset();
  } else {
    // Show the first error
    const firstErr = result.error.issues[0].message;
    formStatus.textContent = "❌ " + firstErr;
    formStatus.style.color = "red";
  }
});
