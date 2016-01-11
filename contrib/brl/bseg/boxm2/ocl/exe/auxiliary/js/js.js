//invokes reel (using id image360)

function scene_frames(frames){
    var stack = []
    var rows = 3, cols = 36
    for(var r=0; r < rows; r+=1){
      for(var c=0; c < cols; c+=1) {
        var name = ['scene_',r,'_',c,'.jpg'].join('')
        stack.push(name)
      }
    }
    return stack
}

