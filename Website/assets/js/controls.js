var scrollingSpeed = 200;

function ScrollHalfPageDown()
{
  window.scrollBy({
    top: 0.5 * window.innerHeight,
    left: 0,
  });
}

function ScrollHalfPageUp()
{
  window.scrollBy({
    top: -0.5 * window.innerHeight,
    left: 0,
  });
}

function handleKeyDown(event) {
  const { ctrlKey, key } = event;
  switch (key) {
    case 'd':
    case 'в':
      {
        if (ctrlKey) {
          ScrollHalfPageDown();
          event.preventDefault();
        }
      }
      break;
    case 'u':
    case 'г':
      {
        if (ctrlKey) {
          ScrollHalfPageUp();
          event.preventDefault();
        }
      }
      break;
    case 'j':
    case 'о':
      {
        window.scrollBy({
          top: 0.5 * scrollingSpeed,
          left: 0,
        });
      }
      break;
    case 'k':
    case 'л':
      {
        window.scrollBy({
          top: -0.5 * scrollingSpeed,
          left: 0,
        });
      }
      break;
    case 'g':
    case 'п':
      {
        window.scrollTo({
          top: 0,
          left: 0,
        });
      }
      break;
    case 'G':
    case 'П':
      {
        window.scrollTo({
          top: document.body.scrollHeight,
          left: 0,
        });
      }
      break;
    default:
      break;
  }

}

function setHandler()
{
  document.addEventListener('keydown', handleKeyDown);
}

if (window.addEventListener) // W3C standard
{
  window.addEventListener('load', setHandler, false); // NB **not** 'onload'
} 
else if (window.attachEvent) // Microsoft
{
  window.attachEvent('onload', setHandler);
}
