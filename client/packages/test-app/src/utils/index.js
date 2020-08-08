export const toBytesInt32 = num => {
  const arr = new ArrayBuffer(4);
  const view = new DataView(arr);
  view.setInt32(0, num, false);
  return arr;
};

export const byteArrayToHexString = uint8arr => {
  if (!uint8arr) {
    return '';
  }

  let hexStr = '';
  for (let  i = 0; i < uint8arr.length; i++) {
    var hex = (uint8arr[i] & 0xff).toString(16);
    hex = hex.length === 1 ? '0' + hex : hex;
    hexStr += hex;
  }

  return hexStr;
};
