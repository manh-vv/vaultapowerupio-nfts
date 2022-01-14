const nftMetadata = {
  genesis: {
    bronze: [
      { key: "name", value: ["string", "EOS PowerUp Genesis Bronze Utility NFT"] },
      { key: "img", value: ["string", ""] },
      { key: "rarity", value: ["string", "Bronze"] },
      { key: "details", value: ["string", "Any account holding this NFT can receive 2x free resouces when claiming free PowerUps."] },
      { key: "url", value: ["string", "https://eospowerup.io"] },
    ],
    silver: [
      { key: "name", value: ["string", "EOS PowerUp Genesis Silver Utility NFT"] },
      { key: "img", value: ["string", ""] },
      { key: "rarity", value: ["string", "Silver"] },
      { key: "details", value: ["string", "Any account holding this NFT can utilize referral links that earn 50% of the fees of any invited accounts. Generate your referral link at eospowerup.io/refferal. Payouts discontinue if the NFT is removed from this account. "] },
      { key: "url", value: ["string", "https://eospowerup.io/refferal"] },
    ],
    gold: [
      { key: "name", value: ["string", "EOS PowerUp Genesis Gold NFT"] },
      { key: "img", value: ["string", "Qma4RhTLzLpVFV3yvDeZ6wM6ZvEiKnTU65RJrTrra9dM1U"] },
      { key: "rarity", value: ["string", "Gold"] },
      { key: "details", value: ["string", "When the account holding this NFT receives Auto-PowerUps (in the watchlist at eospowerup.io/auto) no platform fees will be charged."] },
      { key: "url", value: ["string", "https://eospowerup.io"] },
    ]
  }
}

module.exports = nftMetadata
