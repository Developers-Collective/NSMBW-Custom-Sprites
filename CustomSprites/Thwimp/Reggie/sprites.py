class SpriteImage_Koton(SLib.SpriteImage_StaticMultiple):  # 487
    @staticmethod
    def loadImages():
        if 'Koton0' in ImageCache: return
        for i in range(1):
            ImageCache[f'Koton{i}'] = SLib.GetImg(f'koton_{i}.png')

    def dataChanged(self):

        #color = (self.parent.spritedata[2] & 0xF)
        color = (self.parent.spritedata[2] >> 4) & 15

        self.image = ImageCache[f'Koton{color}']
        self.offset = (-4, -4)

        super().dataChanged()

    487: SpriteImage_Koton,  
