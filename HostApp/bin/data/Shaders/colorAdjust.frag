uniform sampler2DRect image;
uniform float brightness;
uniform float contrast;
uniform float saturation;

varying vec3 pos;

void main (void)
{
    vec2 texCoord = vec2(pos.x, pos.y);
    vec4 col = texture2DRect(image, texCoord);
    
    // Brightness adjustment
    col.rgb += vec3(brightness);
    
    // Contrast adjustment
    col.rgb = (col.rgb - vec3(0.5)) * contrast + vec3(0.5);
    
    // Saturation adjustment
    float luma = dot(col.rgb, vec3(0.299, 0.587, 0.114));
    col.rgb = mix(vec3(luma), col.rgb, saturation);
    
    gl_FragColor = col;
}
