document.addEventListener("DOMContentLoaded", () => {
    console.log("Stefan Mladić - Portfolio loaded");

    const links = document.querySelectorAll('.contact-links a');
    
    links.forEach(link => {
        link.addEventListener('mouseenter', () => {
            link.style.transform = 'scale(1.05) translateY(-2px)';
        });
        
        link.addEventListener('mouseleave', () => {
            link.style.transform = 'scale(1) translateY(0)';
        });
    });
    
    const matrixLink = document.querySelector('.server-note a');
    if (matrixLink) {
        matrixLink.addEventListener('click', () => {
            console.log('Matrix repository link clicked');
        });
    }
});
