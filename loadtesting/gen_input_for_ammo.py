import random
import numpy as np

requests = ["get_best_songs", "get_new_songs", "post_song"]
plan = {"count": int(1e5), "probs": [0.4, 0.4, 0.2]}

limit_min, limit_max = 1, 100
author_name_num_max = 1000
song_name_num_max = int(1e6)
duration_min, duration_max = 10, 600
formats = ['flac', 'mp3', 'aa', 'aax', 'wav']

post_body_template = "{{\"name\": \"{}\", \"author\": \"{}\", \"duration\": \"{}\", \"format\": \"{}\"}}"

def main():
    for i in xrange(plan["count"]):
	req = np.random.choice(np.array(requests), 1, p=plan["probs"])

        if req == "get_best_songs":
            limit = random.randint(limit_min, limit_max)
            print "{}||{}||{}".format('GET', '/songs/best?limit=' + str(limit), 'best_songs')
        elif req == "get_new_songs":
            limit = random.randint(limit_min, limit_max)
            print "{}||{}||{}".format('GET', '/songs/new?limit=' + str(limit), 'new_songs')
        else:
            numauthor = random.randint(1, author_name_num_max)
            numname = random.randint(1, song_name_num_max)
	    duration = random.randint(duration_min, duration_max)
            audioformat = np.random.choice(np.array(formats), 1)[0]

            body = post_body_template.format('Name' + str(numname), 'Author' + str(numauthor), duration, audioformat)
            print  "{}||{}||{}||{}".format('POST', '/songs', 'add_song', body)  
            
if __name__ == "__main__":
    main()
