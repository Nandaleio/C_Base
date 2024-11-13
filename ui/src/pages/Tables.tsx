

export function Tables() {

    const primaryAttribut = {primary: true}
    const secondaryAttribut = {secondary: true}

    return (
        <>
        <h3 class="demo-header">table</h3>
    <table>
      <thead>
        <tr>
          <th>Name</th>
          <th>Home Runs</th>
          <th>AVG</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>Mike Trout</td>
          <td>45</td>
          <td>.291</td>
        </tr>
        <tr>
          <td>Christian Yelich</td>
          <td>44</td>
          <td>.329</td>
        </tr>
        <tr>
          <td>Mookie Betts</td>
          <td>29</td>
          <td>.295</td>
        </tr>
        <tr>
          <td>Cody Bellinger</td>
          <td>47</td>
          <td>.305</td>
        </tr>
      </tbody>
    </table>

    <table {...primaryAttribut}>
        <thead>
          <tr>
            <th>Name</th>
            <th>Home Runs</th>
            <th>AVG</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Mike Trout</td>
            <td>45</td>
            <td>.291</td>
          </tr>
          <tr>
            <td>Christian Yelich</td>
            <td>44</td>
            <td>.329</td>
          </tr>
          <tr>
            <td>Mookie Betts</td>
            <td>29</td>
            <td>.295</td>
          </tr>
          <tr>
            <td>Cody Bellinger</td>
            <td>47</td>
            <td>.305</td>
          </tr>
        </tbody>
      </table>
  
      <table {...secondaryAttribut}>
        <thead>
          <tr>
            <th>Name</th>
            <th>Home Runs</th>
            <th>AVG</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Mike Trout</td>
            <td>45</td>
            <td>.291</td>
          </tr>
          <tr>
            <td>Christian Yelich</td>
            <td>44</td>
            <td>.329</td>
          </tr>
          <tr>
            <td>Mookie Betts</td>
            <td>29</td>
            <td>.295</td>
          </tr>
          <tr>
            <td>Cody Bellinger</td>
            <td>47</td>
            <td>.305</td>
          </tr>
        </tbody>
      </table>
  

    <h3 class="demo-header">button</h3>
    <button type="button">Save</button>
    <button type="button" disabled>Disabled</button>
    <br/>
    <button type="button" {...primaryAttribut}>Save</button>
    <button type="button" {...primaryAttribut} disabled>Disabled</button>
    <br/>
    <button type="button" {...secondaryAttribut}>Save</button>
    <button type="button" {...secondaryAttribut} disabled>Disabled</button>

    <h3 class="demo-header">datalist</h3>
    <label for="ice-cream-choice">Choose a flavor</label>
    <input list="ice-cream-flavors" id="ice-cream-choice" name="ice-cream-choice" />
    <datalist id="ice-cream-flavors">
      <option value="Chocolate"></option>
      <option value="Coconut"></option>
      <option value="Mint"></option>
      <option value="Strawberry"></option>
      <option value="Vanilla"></option>
    </datalist>

    <h3 class="demo-header">fieldset</h3>
    <form>
      <fieldset>
        <legend>Choose your favorite monster</legend>
        <label for="kraken">
          <input type="radio" id="kraken" name="monster" />
          Kraken
        </label>
        <label for="sasquatch">
          <input type="radio" id="sasquatch" name="monster" />
          Sasquatch
        </label>
        <label for="mothman">
          <input type="radio" id="mothman" name="monster" />
          Mothman
        </label>
      </fieldset>
    </form>

    <h3 class="demo-header">form</h3>
    <form action="#" method="get">
      <div class="form-example">
        <label for="name"> Full name </label>
        <input type="text" name="name" id="name" required />
      </div>
      <div class="form-example">
        <label for="email"> Email </label>
        <input type="email" name="email" id="email" required />
      </div>
      <div class="form-example">
        <label for="favnum"> Favorite Number </label>
        <input type="number" id="favnum" name="favnum" value="" />
      </div>
      <div class="form-example">
        <label for="date"> Date </label>
        <input type="date" id="date" name="date" value="2006-01-02" />
      </div>
      <div class="form-example">
        <label for="surprise">
          <input type="checkbox" id="surprise" name="surprise" value="" />
          Add a surprise
        </label>
      </div>
      <div class="form-example">
        <input type="submit" value="Subscribe!" />
      </div>
    </form>

    <h3 class="demo-header">select</h3>
    <label for="pet-select">Choose a pet</label>
    <select name="pets" id="pet-select">
      <option value="dog">Dog</option>
      <option value="cat">Cat</option>
      <option value="hamster">Hamster</option>
      <option value="parrot">Parrot</option>
      <option value="spider">Spider</option>
      <option value="goldfish">Goldfish</option>
    </select>

    <h3 class="demo-header">text area</h3>
    <textarea id="story" name="story" placeholder="It was a dark and stormy night..." rows={7} cols={66}></textarea>

    <h3 class="demo-header">summary/details</h3>
    <details>
      <summary>Details</summary>
      Something small enough to escape casual notice.
    </details>

    <div class="misc-components">
      <div>
        <h3 class="demo-header">meter</h3>
        <label for="fuel">Fuel level</label>
        <meter id="fuel" min="0" max="100" low={33} high={66} optimum={80} value="77"></meter>
      </div>

      <div>
        <h3 class="demo-header">optgroup</h3>
        <label for="dino-select">Choose a dinosaur</label>
        <select id="dino-select">
          <optgroup label="Theropods">
            <option>Tyrannosaurus</option>
            <option>Velociraptor</option>
            <option>Deinonychus</option>
          </optgroup>
          <optgroup label="Sauropods">
            <option>Diplodocus</option>
            <option>Saltasaurus</option>
            <option>Apatosaurus</option>
          </optgroup>
        </select>
      </div>

      <div>
        <h3 class="demo-header">progress</h3>
        <label for="file">File progress</label>
        <progress id="file" max="100" value="70">70%</progress>
      </div>

      <div>
        <h3 class="demo-header" id="rangedemo-label">range/output</h3>
        <form>
          <input type="range" id="rangedemo" name="rangedemo" value="0" aria-labelledby="rangedemo-label" />
          <output name="result" for="b">0</output>
        </form>
      </div>
      
      <div>
        <h3 class="demo-header">color picker</h3>
        <label for="color-picker">Pick a color</label>
        <input type="color" id="color-picker" value="#843AB2" />
      </div>
    </div>
    </>
    )
}